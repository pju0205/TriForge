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

void ATFPlayerCharacter::TESTS()
{
	GEngine->AddOnScreenDebugMessage(30, 1.5f, FColor::Red, TEXT("TEST Called SUC!"));
	GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Yellow, TEXT("[T] Manual Launch Triggered"));

	FVector SafeOffset = GetActorForwardVector() * -50.f;
	SetActorLocation(GetActorLocation() + SafeOffset, false, nullptr, ETeleportType::TeleportPhysics);

	FVector LaunchDir = FVector(1, 0, 1).GetSafeNormal();
	LaunchCharacter(LaunchDir * 1500.f, true, true);

}

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

	if (GetCharacterMovement()->IsFalling())
	{
		if (!bIsWallRunning)
		{
			CheckForWallRun(); // 벽 타고 있지 않을 때만 감지
		}
		else
		{
			// 벽이 여전히 존재하는지 확인
			FVector Start = GetActorLocation();
			FVector DirToCheck = (WallRunSide == EWallRunSide::Right) ? GetActorRightVector() : -GetActorRightVector();
			FVector DummyNormal;

			if (!TraceWall(Start, DirToCheck, 100.f, DummyNormal))
			{
				StopWallRun(); // 벽이 끝나면 떨어짐
			}
		}
	}
	UpdateMovement();
}
void ATFPlayerCharacter::CheckForWallRun()
{
	FVector Start = GetActorLocation();
	FVector Right = GetActorRightVector();
	FVector Left = -Right;

	FVector HitNormal;
	if (TraceWall(Start, Right, 100.f, HitNormal))
	{
		StartWallRun(EWallRunSide::Right, HitNormal);
	}
	else if (TraceWall(Start, Left, 100.f, HitNormal))
	{
		StartWallRun(EWallRunSide::Left, HitNormal);
	}
}

// .cpp - TraceWall 함수 수정
bool ATFPlayerCharacter::TraceWall(const FVector& Start, const FVector& Dir, float Distance, FVector& OutHitNormal)
{
	if (bIsWallRunning) return false; // 벽 타는 중엔 감지 자체 안함

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	FVector End = Start + Dir * Distance;

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		if (Hit.GetActor() == LastWallActor)
			return false; // 같은 벽이면 무시

		OutHitNormal = Hit.ImpactNormal;
		HitWallActor = Hit.GetActor();
		return true;
	}

	return false;
}

void ATFPlayerCharacter::StartWallRun(EWallRunSide Side, const FVector& InWallNormal)
{
	bIsWallRunning = true;
	LastWallActor = HitWallActor;
	WallNormal = InWallNormal;
	WallRunSide = Side;

	FVector Dir = FVector::CrossProduct(WallNormal, FVector::UpVector);
	if (FVector::DotProduct(GetActorForwardVector(), Dir) < 0)
		Dir *= -1;

	GetCharacterMovement()->GravityScale = 0.f;
	GetCharacterMovement()->Velocity = Dir * 600.f;
}

void ATFPlayerCharacter::WallRunJump()
{
	if (!bIsWallRunning) return;

	FVector LaunchDir = (-WallNormal + FVector::UpVector).GetSafeNormal();
	LaunchCharacter(LaunchDir * 1500.f, true, true);

	
	StopWallRun(); // 벽 점프 후 벽타기 종료
}
void ATFPlayerCharacter::StopWallRun()
{
	if (!bIsWallRunning)
		return;

	bIsWallRunning = false;
	WallRunSide = EWallRunSide::None;
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