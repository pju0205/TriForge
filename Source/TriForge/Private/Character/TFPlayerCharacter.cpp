// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TFPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "KismetAnimationLibrary.h"
#include "Character/TFPlayerController.h"
#include "Character/Component/TFPlayerHealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Curves/CurveFloat.h"
#include "Game/TFGameMode.h"
#include "HUD/TFHUD.h"
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
}

void ATFPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateMovement();
}

void ATFPlayerCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATFPlayerCharacter, bSprinting);
	DOREPLIFETIME(ATFPlayerCharacter, bWalking);
	DOREPLIFETIME(ATFPlayerCharacter, bSliding);

	DOREPLIFETIME_CONDITION(ATFPlayerCharacter, OverlappingWeapon, COND_OwnerOnly);
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


// Jump an Lande Start -------------------------
// ChooserTable에서 사용할 데이터를 계산하기 위함
// TFAnimInstance Class에서 JustLandedLight, JustLandedHeavy 함수에서 bJustLanded 데이터를 사용함

void ATFPlayerCharacter::CustomJump()
{
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(7, 2, FColor::Red, "Call CustomJump");
	if (bSliding)
	{
		
		// 슬라이딩 중일 때는 방향성 점프
		FVector Forward = GetActorForwardVector();
		FVector LaunchVelocity = Forward * 600.f + FVector(0, 0, 400.f); // 앞으로 + 위로 튀기기
		Jump();
		LaunchCharacter(LaunchVelocity, true, true);

		// 슬라이딩 중단
		bSliding = false;

	}
	else
	{
		// 일반 점프
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

	AActor* DamageInstigator = nullptr;
	if (IsValid(DamageCauser))
	{
		DamageInstigator = DamageCauser->GetInstigator(); // Pawn이나 Character인 경우 보통 이게 공격자
		if (!IsValid(DamageInstigator))
		{
			DamageInstigator = DamageCauser; // Fallback
		}
	}
	HealthComponent->CalcDamage(Damage, DamageInstigator);
}

void ATFPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (WeaponComponent)
	{
		WeaponComponent->PlayerCharacter = this;
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

	// 죽는 몽타주 실행
	PlayDirectionalDeathMontage(DeathInstigator);
	// 실행 안됨 Montage_Play를 어떻게 실행시키는건지 모르겠네
	
	// 라운드 종료 시키기
	ATFPlayerController* VictimController = Cast<ATFPlayerController>(GetController());
	if (HasAuthority() && IsValid(VictimController))
	{
		// 죽은 유저 가지고 있는 무기 삭제시키기
		if (IsValid(WeaponComponent))
		{
			WeaponComponent->DestroyWeapon();
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

// DeathMontage 실행 함수
void ATFPlayerCharacter::PlayDirectionalDeathMontage(AActor* DeathInstigator)
{
	if (!DeathInstigator) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimInstance Nullptr"));
		return;
	}
	
	FVector HitDirection = (GetActorLocation() - DeathInstigator->GetActorLocation()).GetSafeNormal();
	UAnimMontage* SelectedMontage = GetDirectionalDeathMontage(HitDirection);	// 각도 계산

	if (SelectedMontage)
	{
		AnimInstance->Montage_Play(SelectedMontage);
		UE_LOG(LogTemp, Warning, TEXT("Selected Montage: %s"), *GetNameSafe(SelectedMontage));
	}
}

// 맞은 방향 계산해서 그에 해당하는 Death 몽타주 return
UAnimMontage* ATFPlayerCharacter::GetDirectionalDeathMontage(const FVector& HitDirection) const
{
	FVector Forward = GetActorForwardVector();
	FVector Right = GetActorRightVector();

	float ForwardDot = FVector::DotProduct(Forward, HitDirection);
	float RightDot = FVector::DotProduct(Right, HitDirection);

	if (ForwardDot > 0.7f) return DeathMontage_Front;
	if (ForwardDot < -0.7f) return DeathMontage_Back;
	if (RightDot > 0.7f) return DeathMontage_Right;
	if (RightDot < -0.7f) return DeathMontage_Left;
	if (ForwardDot > 0.f && RightDot > 0.f) return DeathMontage_FrontRight;
	if (ForwardDot > 0.f && RightDot < 0.f) return DeathMontage_FrontLeft;

	// 기본적으로 Front
	return DeathMontage_Front;
}


void ATFPlayerCharacter::CleanupBeforeMapTravel()
{
	if (IsValid(WeaponComponent))
	{
		WeaponComponent->DestroyWeapon();		// 손에 들고 있는 총기 삭제
		WeaponComponent->EquippedWeapon->Destroy();
		WeaponComponent->EquippedWeapon = nullptr;
	}
}
