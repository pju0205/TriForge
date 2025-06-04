// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TFPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Character/TFAnimInstance.h"
#include "Camera/CameraComponent.h"
#include "KismetAnimationLibrary.h"
#include "Character/TFPlayerController.h"
#include "Character/Component/TFPlayerHealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Curves/CurveFloat.h"
#include "Engine/DamageEvents.h"
#include "Game/TFGameMode.h"
#include "HUD/TFHUD.h"
#include "HUD/UI/PlayerHealthBar.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "TriForge/TriForge.h"
#include "Weapon/TFWeapon.h"
#include "Weapon/TFWeaponComponent.h"

ATFPlayerCharacter::ATFPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	MovementComponent->bOrientRotationToMovement = false;
	MovementComponent->RotationRate = FRotator(0.f, 300.f, 0.f);

	// 1인칭 카메라 설정
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetMesh(), FName("head"));
	Camera->bUsePawnControlRotation = true;

	// 3인칭 카메라 설정
	Camera3p = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera3p"));
	Camera3p->SetupAttachment(RootComponent);
	Camera3p->SetRelativeLocation(FVector(-300.f, 0.f, 150.f)); // 캐릭터 뒤 공중 위치
	Camera3p->bUsePawnControlRotation = false;
	Camera3p->SetAutoActivate(false); // 기본은 비활성화

	// Weapon
	WeaponComponent = CreateDefaultSubobject<UTFWeaponComponent>(TEXT("WeaponComponent"));
	WeaponComponent->SetIsReplicated(true);

	// Health 관련
	HealthComponent = CreateDefaultSubobject<UTFPlayerHealthComponent>("Health");
	HealthComponent->SetIsReplicated(true);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	
	WalkSpeed = FVector(400.0f, 375.0f, 350.0f);			// default : 300 275 250
	SprintSpeed = FVector(1000.0f, 775.0f, 750.0f);			// default : 700 575 550
	ECurrentGait = E_Gait::Walk;
	bSprinting = false;
	bWalking = true;
	bJustLanded = false;
	LandVelocity = FVector(0.0f, 0.0f, 0.0f);
	SlideMontage = nullptr;
	bIsFallingDamageApplied = false;
	WallRunState = EWallRunState::None;
}


void ATFPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UpdateMovement();
	
	if (GetCharacterMovement()->IsFalling()) // 공중에 있을 시
	{
		if (WallRunState == EWallRunState::None)  // 현재 벽타기를 하고 있지 않다면 벽타기 체크 하기 (벽타기 중에 체크하면 계속해서 현재 타고있는 벽이 탐지되어서 튕기기 점프가 안됨)
			CheckWallRun(); // 벽 감지 로직 실행
		
		if (GetLastMovementInputVector().IsNearlyZero()) // (슬라이딩) 점프 관련 -> 슬라이딩 점프 혹은 기본 점프 시 방향키 떼면 속도 감소
		{
			FVector Velocity = GetCharacterMovement()->Velocity;
			FVector Horizontal = FVector(Velocity.X, Velocity.Y, 0.f);
			FVector Slowed = FMath::VInterpTo(Horizontal, FVector::ZeroVector, DeltaTime, 2.0f);
			GetCharacterMovement()->Velocity = FVector(Slowed.X, Slowed.Y, Velocity.Z);
		}
	}

	const float KillZ = -1000.f; // Kill 되는 높이

	if (!bIsFallingDamageApplied && GetActorLocation().Z < KillZ)
	{
		bIsFallingDamageApplied = true;	// 데미지 한번만 적용하도록

		FDamageEvent DamageEvent;
		TakeDamage(999.f, DamageEvent, nullptr, nullptr); // nullptr은 피해를 입힌 Controller/Actor

		UE_LOG(LogTemp, Warning, TEXT("Fell below kill height. Applied damage."));
	}
}

void ATFPlayerCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATFPlayerCharacter, bSprinting);
	DOREPLIFETIME(ATFPlayerCharacter, bWalking);
	DOREPLIFETIME(ATFPlayerCharacter, bSliding);
	
	DOREPLIFETIME_CONDITION(ATFPlayerCharacter, OverlappingWeapon, COND_OwnerOnly);
}

void ATFPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ResetToFirstPersonCamera();	// 카메라 1인칭으로 전환
}

void ATFPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ThisClass::ReceiveDamage);
	}

	// Death 관련 함수 바인딩
	if (IsValid(HealthComponent))
	{
		HealthComponent->OnDeathStarted.AddDynamic(this, &ATFPlayerCharacter::OnDeathStarted);
	}
}


// Update Movement Start -----------------------------
// Gait 값을 업데이트하고 이를 사용하여 캐릭터 이동 컴포넌트의 최대 이동 속도를 설정하는 데 사용
void ATFPlayerCharacter::GetDesiredGait()
{
	if (bSprinting)
	{
		ECurrentGait = E_Gait::Sprint;
	}
	else
	{
		if (bWalking)
		{
			ECurrentGait = E_Gait::Walk;
		}
	}
}

// 이동 방향에 따라 다른 속도로 설정 (뒤로 걸으면 속도가 느림)
float ATFPlayerCharacter::CalculateMaxSpeed(float& StrafeSpeedMap)
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	float MovementDirection = UKismetAnimationLibrary::CalculateDirection(MovementComponent->Velocity, GetActorRotation());
	MovementDirection = FMath::Abs(MovementDirection);
	if (StrafeSpeedMapCurve)
	{
		StrafeSpeedMap = StrafeSpeedMapCurve->GetFloatValue(MovementDirection);
	}

	FVector CurrentSpeed = FVector::ZeroVector;
	if (ECurrentGait == E_Gait::Walk)
	{
		CurrentSpeed = WalkSpeed;
	}
	else if (ECurrentGait == E_Gait::Sprint)
	{
		CurrentSpeed = SprintSpeed;
	}
	float FrontSpeed = UKismetMathLibrary::MapRangeClamped(StrafeSpeedMap, 0.0f, 1.0f, CurrentSpeed.X, CurrentSpeed.Y);
	float BackSpeed = UKismetMathLibrary::MapRangeClamped(StrafeSpeedMap, 1.0f, 2.0f, CurrentSpeed.Y, CurrentSpeed.Z);
	float FinalSpeed = UKismetMathLibrary::SelectFloat(FrontSpeed, BackSpeed, StrafeSpeedMap < 1.0);

	return FinalSpeed;
}

void ATFPlayerCharacter::UpdateMovement()
{
	float StrafeSpeedMap = 0.0f;
	
	GetDesiredGait();
	float CurrentSpeed = CalculateMaxSpeed(StrafeSpeedMap);

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	MovementComponent->MaxWalkSpeed = CurrentSpeed;
}
//  ----------------------------- Update Movement End


// Wall Run Start -------------------------------------
void ATFPlayerCharacter::CheckWallRun()
{
	const float TraceDistance = 60.f; // 벽 탐지 거리
	const FVector ActorLocation = GetActorLocation();
	const FVector RightVector = GetActorRightVector();
	FHitResult HitLeft, HitRight;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bLeftHit = false;
	bool bRightHit = false;
	
	if (WallRunState == EWallRunState::None || WallRunState == EWallRunState::RightWall) // 왼쪽 벽 감지
	{
		FVector Start = ActorLocation;
		FVector End = Start - RightVector * TraceDistance;

		// DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 0.1f, 0, 2.0f);

		bLeftHit = GetWorld()->LineTraceSingleByChannel(
			HitLeft, Start, End, ECC_Visibility, Params
		);
	}
	
	if (WallRunState == EWallRunState::None || WallRunState == EWallRunState::LeftWall) // 오른쪽 벽 감지
	{
		FVector Start = ActorLocation;
		FVector End = Start + RightVector * TraceDistance;

		// DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.1f, 0, 2.0f);

		bRightHit = GetWorld()->LineTraceSingleByChannel(
			HitRight, Start, End, ECC_Visibility, Params
		);
	}

	// 디버깅 코드
	// if (GEngine)
	// {
	// 	GEngine->AddOnScreenDebugMessage(102, 1.5f, FColor::Yellow,
	// 		FString::Printf(TEXT("bRightHit = %s"), bRightHit ? TEXT("true") : TEXT("false")));
	// }
	// if (GEngine)
	// {
	// 	GEngine->AddOnScreenDebugMessage(103, 1.5f, FColor::Yellow,
	// 		FString::Printf(TEXT("bLeftHit = %s"), bLeftHit ? TEXT("true") : TEXT("false")));
	// }

	if (bLeftHit) // 왼쪽 벽 탐지 성공 시
	{
		WallRunState = EWallRunState::LeftWall;
		
		// 디버깅 코드
		// if (GEngine)
		// {
		// 	GEngine->AddOnScreenDebugMessage(101, 1.5f, FColor::Cyan, FString::Printf(TEXT("WallRunState = %s"), *UEnum::GetValueAsString(WallRunState)));
		// }
		
		StartWallRun(HitLeft.ImpactNormal);
	} 
	else if (bRightHit)  // 오른쪽 벽 탐지 성공 시
	{
		WallRunState = EWallRunState::RightWall;
		
		// 디버깅 코드
		// if (GEngine)
		// {
		// 	GEngine->AddOnScreenDebugMessage(100, 1.5f, FColor::Cyan, FString::Printf(TEXT("WallRunState = %s"), *UEnum::GetValueAsString(WallRunState)));
		// }
		
		StartWallRun(HitRight.ImpactNormal);
	}
}

void ATFPlayerCharacter::StartWallRun( const FVector& WallNormal)
{
	GetCharacterMovement()->GravityScale = 0.f; // 중력 제거
	
	FVector WallForward;

	if (WallRunState == EWallRunState::LeftWall)
	{
		WallForward = FVector::CrossProduct(WallNormal, FVector::UpVector); // 벽 오른쪽 방향
	}
	else if (WallRunState == EWallRunState::RightWall)
	{
		WallForward = FVector::CrossProduct(FVector::UpVector, WallNormal); // 벽 왼쪽 방향
	}

	WallForward.Normalize();
	GetCharacterMovement()->Velocity = WallForward * 600.f; // 벽따라 앞으로 전진

	// 타이머로 벽 끝나면 자동 해제
	// 0.5초 내에 점프를 눌러야 날아감 (0.1로 줄이면 벽타기 중 점프가 너무 어려워짐)
	GetWorldTimerManager().SetTimer(WallRunTimerHandle, this, &ATFPlayerCharacter::StopWallRun, 0.5f, false);
}

void ATFPlayerCharacter::StopWallRun()
{
	WallRunState = EWallRunState::None; // 벽타기 상태 None으로 변경
	GetCharacterMovement()->GravityScale = 1.f; // 중력 복원
}
//  ------------------------------------- Wall Run End


// Jump an Lande Start -------------------------
// ChooserTable에서 사용할 데이터를 계산하기 위함
// TFAnimInstance Class에서 JustLandedLight, JustLandedHeavy 함수에서 bJustLanded 데이터를 사용함
void ATFPlayerCharacter::CustomJump()
{
	// 벽타기 중 점프 시
	// 내가 바라보고 있는 방향으로 점프
	if (WallRunState != EWallRunState::None)
	{
		// 내가 바라보는 방향 (카메라 기준)
		FRotator ControlRotation = GetControlRotation();
		FVector ForwardDir = ControlRotation.Vector(); // = GetForwardVector()

		// 튕겨나갈 방향: 전방 + 위쪽
		FVector JumpDirection = ForwardDir + FVector::UpVector;
		JumpDirection.Normalize();

		LaunchCharacter(JumpDirection * 900.f, true, true);

		// 디버깅 라인
		DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + JumpDirection * 300.0f, FColor::Red, false, 5.5f, 0, 2.0f);

		StopWallRun();
	}
	
	// 슬라이딩 중 점프시
	// 슬라이딩 종료 -> 점프 애니메이션 출력을 위함
	if (bSliding)
	{
		// 현재 재생 중인 몽타주 확인
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && AnimInstance->Montage_IsPlaying(SlideMontage))
		{
			// 슬라이딩 몽타주 중지
			AnimInstance->Montage_Stop(0.1f, SlideMontage);
		}
		
		bSliding = false;
		Jump();
	}

	// 기본 점프 시
	else
	{
		Jump();
	}
}

void ATFPlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();

	LandVelocity = MovementComponent->Velocity;

	bJustLanded = true;

	FLatentActionInfo LatentActionInfo;
	LatentActionInfo.CallbackTarget = this;
	LatentActionInfo.ExecutionFunction = FName("OnDelayComplete");
	LatentActionInfo.Linkage = 0;
	LatentActionInfo.UUID = __LINE__; // 각 지연 액션마다 고유 ID 필요
	
	UKismetSystemLibrary::RetriggerableDelay(this, 0.3f, LatentActionInfo);

}

void ATFPlayerCharacter::OnDelayComplete()
{
	// 0.3초 후에 Just Landed를 false로 설정
	bJustLanded = false;
}
// ------------------------- Jump an Lande End


// Walk and Sprint Start --------------------------
void ATFPlayerCharacter::UpdateSprintState(bool bSprint)
{

	/*	if (HasAuthority()) // 나는 서버이고, 내 캐릭터니까 바로 상태 갱신
		 {
			bSprinting = bSprint;
			bWalking = !bSprint;
		}
		else if (IsLocallyControlled()) // 클라이언트일 경우 → 서버에 요청
		{
			/ServerUpdateSprintState(bSprint);
		}
		=> 서버인 플레이어가 없는 데디게이트 서버일 때는 사용 X */


	// 로컬 입력 받는 클라이언트에서만 서버에 요청하도록 함
	// 일종의 안정 장치. (내가 내 캐릭터만 조작 할 수 있도록 함)
	if (IsLocallyControlled())
	{
		// Server에 값을 변경할 것이라 요청
		// 내 변수 값만 바뀌면 다른 플레이은 모르기 때문.
		ServerUpdateSprintState(bSprint);
	}
}

void ATFPlayerCharacter::ServerUpdateSprintState_Implementation(bool bSprint)
{
	// 서버에서 모든 값들을 바꿈
	// 이때 이 값들은 DOREPLIFETIME 되어 있기 때문에 모든 클라이언트에 복사됨
	if (bSprinting != bSprint)
	{
		bSprinting = bSprint;
		bWalking = !bSprint;
	}
}
// -------------------------- Walk and Sprint End


// Slide Montage Start -------------------
void ATFPlayerCharacter::PlaySlidMontage()
{
	// 조건을 만족할 때만 서버에 요청
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	float Velocity = UKismetMathLibrary::VSize(MovementComponent->Velocity);

	if (Velocity > 1.0f && IsLocallyControlled())
	{
		ServerRequestSlide();
	}
}

void ATFPlayerCharacter::ServerRequestSlide_Implementation()
{
	// Velocity 체크 - 서버에서도 안전하게 조건 확인
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	float Velocity = UKismetMathLibrary::VSize(MovementComponent->Velocity);

	if (Velocity > 1.0f)
	{
		bSliding = true; // 슬라이딩 상태 시작
		MulticastPlaySlideMontage();
	}
}

void ATFPlayerCharacter::MulticastPlaySlideMontage_Implementation()
{
	if (SlideMontage && GetMesh())
	{
		
		float Duration = PlayAnimMontage(SlideMontage);

		// 슬라이딩 종료 예약
		FTimerHandle SlideEndTimerHandle;
		GetWorldTimerManager().SetTimer(SlideEndTimerHandle, [this]()
		{
			bSliding = false;
		}, Duration, false);
	}
}
// -------------- Slide Montage End


// 데미지 처리 함수
void ATFPlayerCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
	class AController* InstigatedBy, AActor* DamageCauser)
{

	if (!IsValid(HealthComponent)) return;

	AActor* DamageInstigator = InstigatedBy ? InstigatedBy->GetPawn() : nullptr;

	HealthComponent->CalcDamage(Damage, DamageInstigator);
}

void ATFPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (WeaponComponent)
	{
		WeaponComponent->PlayerCharacter = this;
	}

	if (HealthComponent)
	{
		
	}
}

void ATFPlayerCharacter::SetOverlappingWeapon(ATFWeapon* Weapon)
{
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

bool ATFPlayerCharacter::IsWeaponEquipped()
{
	return (WeaponComponent && WeaponComponent->EquippedWeapon);
}

bool ATFPlayerCharacter::IsAiming()
{
	return (WeaponComponent && WeaponComponent->bAiming);
}

void ATFPlayerCharacter::AimButtonPressed()
{
	if (WeaponComponent)
	{
		WeaponComponent->SetAiming(true);
	}
}

void ATFPlayerCharacter::AimButtonReleased()
{
	if (WeaponComponent)
	{
		WeaponComponent->SetAiming(false);
	}
}

void ATFPlayerCharacter::EquipButtonPressed()
{
	if (WeaponComponent)
	{
		ServerEquipButtonPressed();
	}
}

void ATFPlayerCharacter::ServerEquipButtonPressed_Implementation()
{
	if (WeaponComponent)
	{
		MulticastEquipButtonPressed();
	}
}

void ATFPlayerCharacter::MulticastEquipButtonPressed_Implementation()
{
	if (WeaponComponent)
	{
		WeaponComponent->EquipWeapon(OverlappingWeapon);
	}
}


void ATFPlayerCharacter::AttackButtonPressed()
{
	if (WeaponComponent)
	{
		WeaponComponent->AttackButtonPressed(true);
	}
}

void ATFPlayerCharacter::AttackButtonReleased()
{
	if (WeaponComponent)
	{
		WeaponComponent->AttackButtonPressed(false);
	}
}

UTFWeaponComponent* ATFPlayerCharacter::GetWeaponComponent()
{
	return WeaponComponent;
}

ATFWeapon* ATFPlayerCharacter::GetEquippedWeapon()
{
	if (WeaponComponent == nullptr) return nullptr;
	return WeaponComponent->EquippedWeapon;
}

/*void ATFPlayerCharacter::DroppedWeapon()
{
	if (WeaponComponent)
	{
		WeaponComponent->ClientResetAiming();
		WeaponComponent->DropWeapon();
	}
}*/

void ATFPlayerCharacter::OnRep_OverlappingWeapon(ATFWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

void ATFPlayerCharacter::OnDeathStarted(AActor* DyingActor, AActor* DeathInstigator)
{
	if (!HealthComponent || HealthComponent->DeathState == EDeathState::NotDead || HealthComponent->DeathCause == EDeathCause::Unknown) return;
	
	// Ragdoll 실행
	EnableRagdoll();

	// 죽었을 때 카메라 전환
	SwitchToDeathCamera();
	
	// 라운드 종료 시키기
	ATFPlayerController* VictimController = Cast<ATFPlayerController>(GetController());
	if (HasAuthority() && IsValid(VictimController))
	{
		// 죽은 유저 가지고 있는 무기 삭제시키기
		if (IsValid(WeaponComponent))
		{
			WeaponComponent->DropWeapon();
		}

		// 죽은 유저 생존 여부 갱신
		VictimController->bPawnAlive = false;

		// 게임모드에서 진행 갱신
		if (ATFGameMode* GameMode = Cast<ATFGameMode>(UGameplayStatics::GetGameMode(this)))
		{
			ACharacter* InstigatorCharacter = Cast<ACharacter>(DeathInstigator);
			if (IsValid(InstigatorCharacter))
			{
				APlayerController* InstigatorController = Cast<APlayerController>(InstigatorCharacter->GetController());
				if (IsValid(InstigatorController))
				{
					// 라운드 결과 집계
					GameMode->HandleRoundEnd(VictimController, InstigatorController);
				}
			}
			else // 낙사 되면 가해자 없음 -> nullptr 넣어주기
			{
				GameMode->HandleRoundEnd(VictimController, nullptr);
			}
		}
	}

	// 입력 비활성화
	if (IsValid(VictimController))
	{
		DisableInput(VictimController);
	}
	
	// 캐릭터 Collision 처리
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	// 움직임, 입력 등 비활성화
	GetCharacterMovement()->DisableMovement();
}

void ATFPlayerCharacter::SwitchToDeathCamera()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC && Camera3p)
	{
		PC->SetViewTargetWithBlend(this, 0.5f); // 부드럽게 전환
		Camera->Deactivate(); // 1인칭 카메라 끔
		Camera3p->Activate(); // 3인칭 카메라 켬
	}
}

void ATFPlayerCharacter::ResetToFirstPersonCamera()
{
	if (Camera3p) Camera3p->Deactivate();
	if (Camera) Camera->Activate();
}

void ATFPlayerCharacter::EnableRagdoll()
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp) return;

	// Physics 활성화
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetCollisionProfileName(TEXT("Ragdoll"));

	// 캡슐 비활성화
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 현재 위치를 기준으로 물리 적용
	FName PelvisBone = TEXT("pelvis"); // 스켈레톤에 따라 다를 수 있음
	FVector Impulse = FVector(0.f, 0.f, 0.f);
	MeshComp->AddImpulseToAllBodiesBelow(Impulse, PelvisBone, true);
}
