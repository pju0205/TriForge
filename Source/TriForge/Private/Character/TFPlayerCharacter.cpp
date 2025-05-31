// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TFPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "KismetAnimationLibrary.h"
#include "Character/TFPlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Curves/CurveFloat.h"
#include "Net/UnrealNetwork.h"

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
	
	
	WalkSpeed = FVector(300.0f, 275.0f, 250.0f);
	SprintSpeed = FVector(700.0f, 575.0f, 550.0f);
	ECurrentGait = E_Gait::Walk;
	bSprinting = false;
	bWalking = true;
	bSliding = false;
	bJustLanded = false;
	LandVelocity = FVector(0.0f, 0.0f, 0.0f);
	SlideMontage = nullptr;


	WallRunSide = EWallRunSide::None;
}

void ATFPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 벽타기 중 W 떼면 중단
	if (bIsWallRunning)
	{
		float ForwardValue = GetInputAxisValue("MoveForward");
		if (FMath::IsNearlyZero(ForwardValue))
		{
			StopWallRun();
			return;
		}
	}

	// 점프 중일 때 벽 체크
	if (GetCharacterMovement()->IsFalling())
	{
		CheckForWallRun();
	}
	UpdateMovement();
}
void ATFPlayerCharacter::CheckForWallRun()
{
	if (bIsWallRunning)
		return;

	FVector Start = GetActorLocation();
	FVector RightDir = GetActorRightVector();
	FVector LeftDir = -RightDir;
	float TraceDistance = 100.f;

	if (TraceWall(Start, RightDir, TraceDistance, WallNormal))
	{
		StartWallRun(EWallRunSide::Right);
	}
	else if (TraceWall(Start, LeftDir, TraceDistance, WallNormal))
	{
		StartWallRun(EWallRunSide::Left);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No wall detected for wall run"));
	}
}

bool ATFPlayerCharacter::TraceWall(const FVector& Start, const FVector& Direction, float Distance, FVector& OutHitNormal)
{
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	FVector End = Start + Direction * Distance;

	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

	if (bHit && Hit.bBlockingHit)
	{
		OutHitNormal = Hit.ImpactNormal;
		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1.0f);
		return true;
	}

	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.0f);
	return false;
}

void ATFPlayerCharacter::StartWallRun(EWallRunSide Side)
{
	bIsWallRunning = true;
	WallRunSide = Side;

	// 달릴 방향 계산 + 뒤로 가는 문제 방지
	WallRunDirection = FVector::CrossProduct(WallNormal, FVector::UpVector);
	if (FVector::DotProduct(GetActorForwardVector(), WallRunDirection) < 0)
	{
		WallRunDirection *= -1;
	}

	// 중력 제거
	GetCharacterMovement()->GravityScale = 0.0f;

	// 일정 속도로 벽 따라 이동
	GetCharacterMovement()->Velocity = WallRunDirection * 600.f;
}

void ATFPlayerCharacter::WallRunJump()
{
	if (!bIsWallRunning)
		return;

	StopWallRun();

	// 튕겨나가는 방향: 반대 벽 + 위로
	FVector JumpDirection = -WallNormal + FVector::UpVector;
	JumpDirection.Normalize();

	LaunchCharacter(JumpDirection * 800.f, true, true);

	UE_LOG(LogTemp, Warning, TEXT("Wall Run Jump Executed"));
}

void ATFPlayerCharacter::StopWallRun()
{
	if (!bIsWallRunning)
		return;

	bIsWallRunning = false;
	WallRunSide = EWallRunSide::None;

	// 중력 복구
	GetCharacterMovement()->GravityScale = 1.0f;

	UE_LOG(LogTemp, Warning, TEXT("Wall Run Stopped"));
}

void ATFPlayerCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATFPlayerCharacter, bSprinting);
	DOREPLIFETIME(ATFPlayerCharacter, bWalking);
}


void ATFPlayerCharacter::GetDesiredGait()
{
	if (bSprinting || bSliding)
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

void ATFPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ATFPlayerCharacter::UpdateSprintState(bool bSprint)
{
	if (IsLocallyControlled())
	{
		ServerUpdateSprintState(bSprint);
	}
}

void ATFPlayerCharacter::ServerUpdateSprintState_Implementation(bool bSprint)
{
	bSprinting = bSprint;
	bWalking = !bSprint;
}

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