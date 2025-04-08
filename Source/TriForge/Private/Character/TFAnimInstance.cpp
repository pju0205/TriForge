// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TFAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AnimationWarpingLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "PoseSearch/PoseSearchTrajectoryTypes.h"
#include "PoseSearch/PoseSearchTrajectoryLibrary.h"

UTFAnimInstance::UTFAnimInstance()
{
	TFPlayerCharacter = nullptr;
	TFCharacterMovement = nullptr;
	MovementMode = E_MovementMode::OnGorund;
	RotationMode = E_RotationMode::Strafe;
	MovementState = E_MovementState::Idle;
	Gait = E_Gait::Walk;
	bHasAcceleration = false;
	bHasVelocity = false;
	AccelerationAmount = 0.0f;
	Speed2D = 0.0f;
}

void UTFAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	TFPlayerCharacter = Cast<ATFPlayerCharacter>(TryGetPawnOwner());
	if (TFPlayerCharacter != nullptr)
	{
		TFCharacterMovement = TFPlayerCharacter->GetCharacterMovement();
	}
}

void UTFAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (!TFPlayerCharacter)
	{
		TFPlayerCharacter = Cast<ATFPlayerCharacter>(TryGetPawnOwner());
		if (TFPlayerCharacter)
		{
			TFCharacterMovement = TFPlayerCharacter->GetCharacterMovement();
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("TFCharacterMovement"));
		}
	}

	// 디버깅 메시지 조건부 출력
	if (!TFCharacterMovement && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("TFCharacterMovement is null"));
		return; // 이 시점에서 로직 중단하는 게 안정적
	}
	else
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("TFCharacterMovement okokokokokokoko"));
	UpdateEssentialValues();
	GenerateTrajectory(DeltaTime);
	UpdateStates();
}

void UTFAnimInstance::SetRootTransform()
{
	SetOffsetRootNode();

	FTransform OffsetRootNodeTransform = UAnimationWarpingLibrary::GetOffsetRootTransform(OffsetRootNode);
	
	FVector OffsetRootNode_Location = OffsetRootNodeTransform.GetLocation();
	FRotator OffsetRootNode_Rotation = OffsetRootNodeTransform.Rotator();
	FRotator Make_OffsetRootNode_Rotation = FRotator(OffsetRootNode_Rotation.Pitch, OffsetRootNode_Rotation.Yaw, OffsetRootNode_Rotation.Roll + 90.0f);

	RootTransform = UKismetMathLibrary::MakeTransform(OffsetRootNode_Location, Make_OffsetRootNode_Rotation);
}

void UTFAnimInstance::SetAcceleration()
{
	Acceleration = TFCharacterMovement->GetCurrentAcceleration();
	AccelerationAmount = UKismetMathLibrary::VSize(Acceleration) / TFCharacterMovement->GetMaxAcceleration();
	if (AccelerationAmount > 0.0f)
	{
		bHasAcceleration = true;
	}
	else
	{
		bHasAcceleration = false;
	}
}

void UTFAnimInstance::SetVelocity()
{
	VelocityLastFrame = Velocity;
	Velocity = TFCharacterMovement->Velocity;

	Speed2D = UKismetMathLibrary::VSizeXY(Velocity);
	if (Speed2D > 5.0f)
	{
		bHasVelocity = true;
	}
	else
	{
		bHasVelocity = false;
	}

	float DeltaSeconds = UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
	FVector DeltaVelocity = Velocity - VelocityLastFrame;
	VelocityAcceleration = DeltaVelocity / UKismetMathLibrary::FMax(DeltaSeconds, 0.001f);

	if (bHasVelocity)
	{
		LastNonZeroVelocity = Velocity;
	}
}

void UTFAnimInstance::UpdateEssentialValues()
{
	if (TFPlayerCharacter)
	{
		CharacterTransform = TFPlayerCharacter->GetActorTransform();
		UE_LOG(LogTemp, Warning, TEXT("TFPlayerCharacter okokokokokokokokokokokokokokokokokokokokokokokokokok"));
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("TFPlayerCharacter is nullnullnullnullnullnullnullnullnullnullnullnullnull"));
	SetRootTransform();
	
	if (TFCharacterMovement)
	{
		SetAcceleration();
		SetVelocity();
	}
	
	// CurrentDatabaseTags = CurrentSelectedDatabase->Tags;
}

void UTFAnimInstance::GenerateTrajectory(float DeltaTime)
{
	USkeletalMeshComponent* MeshComp = GetSkelMeshComponent();
	UAnimInstance* AnimInst = nullptr;
		
	if (MeshComp)
	{
		AnimInst = MeshComp->GetAnimInstance();
		if (AnimInst)
		{
			FPoseSearchTrajectoryData InTrajectoryData;
			if (Speed2D > 0.0f)
			{
				InTrajectoryData = TrajectoryGenerationDataMoving;
			}
			else
			{
				InTrajectoryData = TrajectoryGenerationDataMoving;
			}
			UPoseSearchTrajectoryLibrary::PoseSearchGenerateTrajectory(
				AnimInst,
				InTrajectoryData,
				DeltaTime,
				Trajectory,
				 PreviousDesiredControllerYaw,
				 Trajectory,
				 -1.0f,
				 30,
				 0.1f,
				 15
				 );
		}
	
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(TFPlayerCharacter);
		UPoseSearchTrajectoryLibrary::HandleTrajectoryWorldCollisions(
			this,
			this,
			Trajectory,
			true,
			0.01f,
			Trajectory,
			TrajectoryCollision,
			ETraceTypeQuery::TraceTypeQuery1,
			false,
			ActorsToIgnore,
			EDrawDebugTrace::None,
			true,
			150.0f
			);

		FPoseSearchQueryTrajectorySample FirstOutTrajectorySample;
		FPoseSearchQueryTrajectorySample SecondsOutTrajectorySample;
		UPoseSearchTrajectoryLibrary::GetTrajectorySampleAtTime(
			Trajectory,          // InTrajectory
			0.5f,                // Time
			FirstOutTrajectorySample, // OutTrajectorySample (ref 파라미터로 넘겨야 함)
			false                // bExtrapolate
		);
		UPoseSearchTrajectoryLibrary::GetTrajectorySampleAtTime(
		Trajectory,          // InTrajectory
		0.4f,                // Time
		SecondsOutTrajectorySample, // OutTrajectorySample (ref 파라미터로 넘겨야 함)
		false                // bExtrapolate
		);
		FutureVelocity = (FirstOutTrajectorySample.Position - SecondsOutTrajectorySample.Position) * 10.0f;
	}
}

void UTFAnimInstance::UpdateStates()
{
	if (TFCharacterMovement)
	{
		MovementModeLastFrame = MovementMode;
		EMovementMode CurrentMovementMode = TFCharacterMovement->MovementMode;
		switch(CurrentMovementMode)
		{
		case EMovementMode::MOVE_None:
		case EMovementMode::MOVE_Walking:
		case EMovementMode::MOVE_NavWalking:
				MovementMode = E_MovementMode::OnGorund;
			break;

		case EMovementMode::MOVE_Falling:
			MovementMode = E_MovementMode::InAir;
			break;

		default:
			break;
		}

		RotationModeLastFrame = RotationMode;
		if (TFCharacterMovement->bOrientRotationToMovement)
		{
			RotationMode = E_RotationMode::OrientToMovement;
		}
		else if (!TFCharacterMovement->bOrientRotationToMovement)
		{
			RotationMode = E_RotationMode::Strafe;
		}
	
		MovementStateLastFrame = MovementState;
		bool bisMoving = isMoving();
		if (bisMoving)
		{
			MovementState = E_MovementState::Moving;
		}
		else
		{
			MovementState = E_MovementState::Idle;
		}

		GaitLastFrame = Gait;
		if (TFPlayerCharacter->GetGait() == E_Gait::Walk)
		{
			Gait = E_Gait::Walk;
		}
		else if (TFPlayerCharacter->GetGait() == E_Gait::Sprint)
		{
			Gait = E_Gait::Sprint;
		}
	}
}

bool UTFAnimInstance::isMoving()
{
	if (UKismetMathLibrary::NotEqual_VectorVector(Velocity, FVector(0.0f, 0.0f, 0.0f), 0.1f) &&
	UKismetMathLibrary::NotEqual_VectorVector(FutureVelocity, FVector(0.0f, 0.0f, 0.0f), 0.1f))
	{
		return true;	
	}
	return false;
}
