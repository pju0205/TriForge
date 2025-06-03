// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TFAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AnimationWarpingLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "PoseSearch/PoseSearchTrajectoryTypes.h"
#include "PoseSearch/PoseSearchTrajectoryLibrary.h"
#include "PoseSearch/PoseSearchDatabase.h"
#include "Weapon/TFRangedWeapon.h"
#include "Weapon/TFWeapon.h"


UTFAnimInstance::UTFAnimInstance()
{
	TFPlayerCharacter = nullptr;
	TFCharacterMovement = nullptr;
	MovementMode = E_MovementMode::OnGorund;
	RotationMode = E_RotationMode::Strafe;
	MovementState = E_MovementState::Idle;
	
	WeaponTypeState = E_EquippedWeaponType::UnEquipped;
	
	Gait = E_Gait::Walk;
	bHasAcceleration = false;
	bHasVelocity = false;
	AccelerationAmount = 0.0f;
	Speed2D = 0.0f;
	HeayLandSpeedThreshold = 700.0f;
	bSliding = false;

	TrajectoryGenerationDataIdle = FPoseSearchTrajectoryData(0.0, 100.0 ,0.0 );
	TrajectoryGenerationDataMoving = FPoseSearchTrajectoryData(0.0, 0.0 ,0.0 );
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
		}
	}
	
	if (TFPlayerCharacter == nullptr) return;

	// 50번 ~ 59번줄(Super 이후부터 주석코드 전) 까지 없애고 주석 처리된 부분으로 사용해도 될 듯
	/*TFPlayerCharacter = TFPlayerCharacter == nullptr ? Cast<ATFPlayerCharacter>(TryGetPawnOwner()) : TFPlayerCharacter;
	if (TFPlayerCharacter == nullptr) return;
	TFCharacterMovement = TFPlayerCharacter->GetCharacterMovement();*/
	
	bWeaponEquipped = TFPlayerCharacter->IsWeaponEquipped();
	EquippedWeapon = TFPlayerCharacter->GetEquippedWeapon();
	if (bWeaponEquipped)
	{
		bRangedWeapon = EquippedWeapon->GetWeaponClass() == EWeaponClass::Ewc_RangedWeapon;
	}
	bSliding = TFPlayerCharacter->GetIsSliding();
	
	UpdateEssentialValues();
	GenerateTrajectory(DeltaTime);
	UpdateStates();

	// 무기 왼손위치를 무기의 LeftHandSocket을 만들어 고정 시키기 위한 함수
	// LeftHandSocket Transform을 월드 상에서 구한 후 BoneSpace에서 오른 손에 대한 상대적위치로 변환 후
	// OutPosition, OutRotation으로 LeftHand를 이동.
	if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && TFPlayerCharacter->GetMesh() && bRangedWeapon)
	{
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		TFPlayerCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));

		// 무기가 Crosshair를 향하도록 하는 코드, RightHandRotation을 이용하여 Transform Bone노드를 이용해 AnimBlueprint에서 적용 가능
		// 본인에게만 적용되고 클라이언트에서는 적용 안 됨, FHitResult를 Replicate되게 하면 할 수 있는데 네트워크 량이 너무 많아짐
		/*if (TFPlayerCharacter->IsLocallyControlled())
		{ 
			ATFRangedWeapon* RangedWeapon = Cast<ATFRangedWeapon>(EquippedWeapon);
			FHitResult Hit;
			RangedWeapon->TraceEnemy(Hit);
			FTransform RightHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("hand_r"), RTS_World);
			RightHandRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(),
				RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - Hit.ImpactPoint));
		}*/
	}

	if (CurrentSelectedDatabase != nullptr)
	{
		// GEngine->AddOnScreenDebugMessage(
		// 	1, 1.5f, FColor::Yellow,
		// 	FString::Printf(TEXT("Tag Count: %d"), CurrentSelectedDatabase->Tags.Num())
		// );
		
		GEngine->AddOnScreenDebugMessage(200, 1.5f, FColor::Red, FString::Printf(TEXT("Database: %s"), *CurrentSelectedDatabase->GetName()));

		for (const auto& Tag : CurrentSelectedDatabase->Tags)
		{
			// GEngine->AddOnScreenDebugMessage(
			// 	3, // -1이면 새로운 메시지로 출력
			// 	1.5f,
			// 	FColor::Blue,
			// 	FString::Printf(TEXT("Database: %s | Tag: %s"), *CurrentSelectedDatabase->GetName(), *Tag.ToString())
			// );
		}
	}

	
}

void UTFAnimInstance::SetRootTransform()
{
	OffsetRootBoneEnabled = UKismetSystemLibrary::GetConsoleVariableBoolValue(FString("a.animnode.offsetrootbone.enable"));

	if (OffsetRootBoneEnabled)
	{
		// Animation Blueprint에서 실행됨
		SetOffsetRootNode();
		
		// 언리얼에서 캐릭터의 Z축은 애니메이션 파일들에 비해 대게 90도 틀어져있음
		// OffsetRoot Node의 기능이 애니메이션 실행 시 루트 본을 캐릭터의 캡슐(캐릭터의 트랜스폼)과 맞추어 주는것
		// -> 여기서 애니메이션 재생 시 루트 본의 위치와 회전은 현재 캐릭터의 캡슐에 맞게 조정 된것
		// -> 그렇기 때문에 이후 90도 틀어진 것을 맞춰주기 위해 90도 더한 TransFrom을 주는 것
		FTransform OffsetRootNodeTransform = UAnimationWarpingLibrary::GetOffsetRootTransform(OffsetRootNode);
		FVector OffsetLocation = OffsetRootNodeTransform.GetLocation();
		FRotator OffsetRotation = OffsetRootNodeTransform.Rotator();
		
		FRotator AdjustedRotation = FRotator(OffsetRotation.Pitch, OffsetRotation.Yaw + 90.0f, OffsetRotation.Roll);

		// ***** 오류 해결 :  머리가 90도 돌아가있던 가장 큰 이유 중 하나 *****
		// OffsetRoot 기능은 5.5부터 멀티 지원
		// OffsetRoot Node로 변경된 Transform은 다른 클라에서 모름. 그래서 직접 멀티 처리를 해줘야 함.  
		// 그래서 Transform 값을 멀티로 처리해서 동기화 하니까 정상 작동 
		FTransform LocalRootTransform = FTransform(AdjustedRotation, OffsetLocation);
		
		if (TFPlayerCharacter->IsLocallyControlled())
		{
			RootTransform = LocalRootTransform;
		}

		// // 디버깅 메시지 출력
		// if (GEngine)
		// {
		// 	const FRotator CharacterRotation = CharacterTransform.Rotator();
		// 	const FRotator OffsetRotation_ = OffsetRootNodeTransform.Rotator();
		// 	const FRotator FinalRootRotation = RootTransform.Rotator();
		//
		// 	const FString ControlType = TFPlayerCharacter->IsLocallyControlled() ? TEXT("[Local]") : TEXT("[Remote]");
		// 	const FString OwnerName = TFPlayerCharacter->GetName();
		//
		// 	GEngine->AddOnScreenDebugMessage(700, 3.f, FColor::Cyan,
		// 		FString::Printf(TEXT("%s %s | CharacterRotation: Pitch=%.1f Yaw=%.1f Roll=%.1f"),
		// 			*ControlType, *OwnerName,
		// 			CharacterRotation.Pitch, CharacterRotation.Yaw, CharacterRotation.Roll));
		//
		// 	GEngine->AddOnScreenDebugMessage(701, 3.f, FColor::Yellow,
		// 		FString::Printf(TEXT("%s %s | OffsetRotation:   Pitch=%.1f Yaw=%.1f Roll=%.1f"),
		// 			*ControlType, *OwnerName,
		// 			OffsetRotation_.Pitch, OffsetRotation_.Yaw, OffsetRotation_.Roll));
		//
		// 	GEngine->AddOnScreenDebugMessage(702, 3.f, FColor::Green,
		// 		FString::Printf(TEXT("%s %s | FinalRootRotation: Pitch=%.1f Yaw=%.1f Roll=%.1f"),
		// 			*ControlType, *OwnerName,
		// 			FinalRootRotation.Pitch, FinalRootRotation.Yaw, FinalRootRotation.Roll));
		//
		// 	// ReplicatedRootTransform 비교 디버깅
		// 	const FRotator ReplicatedRootRotation = TFPlayerCharacter->ReplicatedRootTransform.Rotator();
		// 	GEngine->AddOnScreenDebugMessage(703, 3.f, FColor::Magenta,
		// 		FString::Printf(TEXT("%s %s | ReplicatedRootRotation: Pitch=%.1f Yaw=%.1f Roll=%.1f"),
		// 			*ControlType, *OwnerName,
		// 			ReplicatedRootRotation.Pitch, ReplicatedRootRotation.Yaw, ReplicatedRootRotation.Roll));
		// }
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Emerald, TEXT("OffsetRootBoneEnabled False"));

		RootTransform = CharacterTransform;
	}
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
	if (TFCharacterMovement && TFPlayerCharacter)
	{
		CharacterTransform = TFPlayerCharacter->GetActorTransform();
	
		SetRootTransform();
		SetAcceleration();
		SetVelocity();
		if (CurrentSelectedDatabase)
		{
			CurrentDatabaseTags.Append(CurrentSelectedDatabase->Tags);
		}
		
	}
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
				InTrajectoryData = TrajectoryGenerationDataIdle;
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
		/*UE_LOG(LogTemp, Warning, TEXT("bSprinting: %d, bWalking: %d, Gait: %d"), TFPlayerCharacter->GetIsSprinting(), TFPlayerCharacter->GetIsSWaking(), TFPlayerCharacter->GetGait());*/
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
		
		if (bWeaponEquipped)
		{
			EWeaponType WeaponType = EquippedWeapon->GetWeaponType();
			WeaponTypeState = CheckWeaponType(WeaponType);
		}
		else
		{
			WeaponTypeState = E_EquippedWeaponType::UnEquipped;
		}
	}
}

E_EquippedWeaponType UTFAnimInstance::CheckWeaponType(EWeaponType CurrentWeaponType)
{
	E_EquippedWeaponType EquippedWeaponType;
	switch (CurrentWeaponType)
	{
	case EWeaponType::Ewt_Rifle:
		EquippedWeaponType = E_EquippedWeaponType::Rifle;
		break;
	case EWeaponType::Ewt_Pistol:
		EquippedWeaponType = E_EquippedWeaponType::Pistol;
		break;
	case EWeaponType::Ewt_ShotGun:
		EquippedWeaponType = E_EquippedWeaponType::Rifle;
		break;
	case EWeaponType::Ewt_SniperRifle:
		EquippedWeaponType = E_EquippedWeaponType::Rifle;
		break;
	case EWeaponType::Ewt_Knife:
		EquippedWeaponType = E_EquippedWeaponType::UnEquipped;
		break;
	case EWeaponType::Ewt_Hammer:
		EquippedWeaponType = E_EquippedWeaponType::UnEquipped;
		break;
	default:
		EquippedWeaponType = E_EquippedWeaponType::UnEquipped;
		break;
	}
	return EquippedWeaponType;
}

bool UTFAnimInstance::isMoving()
{
	if (UKismetMathLibrary::NotEqual_VectorVector(Velocity, FVector(0.0f, 0.0f, 0.0f), 0.1f) &&
	UKismetMathLibrary::NotEqual_VectorVector(FutureVelocity, FVector(0.0f, 0.0f, 0.0f), 0.1f))
	{
		// if (GEngine)
		// 	GEngine->AddOnScreenDebugMessage(40, 1, FColor::Yellow, "Moving true");
		return true;	
	}
	// if (GEngine)
	// 	GEngine->AddOnScreenDebugMessage(40, 1, FColor::Yellow, "Moving False");
	return false;
}

bool UTFAnimInstance::isStarting()
{
	bool bisMoving = isMoving();
	bool bHasPivotTag = !CurrentDatabaseTags.Contains("Pivots");
	bool bisStarting = false;
	if (UKismetMathLibrary::VSizeXY(FutureVelocity) >= UKismetMathLibrary::VSizeXY(Velocity) + 100.0f)
	{
		bisStarting = true;
	}
	
	// GEngine->AddOnScreenDebugMessage(4, 1, FColor::Green,bisMoving ? TEXT("bisMoving : true") : TEXT("bisMoving : false"));
	// GEngine->AddOnScreenDebugMessage(5, 1, FColor::Green,bHasPivotTag ? TEXT("bHasPivotTag : true") : TEXT("bHasPivotTag : false"));
	// GEngine->AddOnScreenDebugMessage(6, 1, FColor::Green,bisStarting ? TEXT("bisStarting : true") : TEXT("bisStarting : false"));
	return bisMoving && bHasPivotTag && bisStarting;
}

bool UTFAnimInstance::isPivoting()
{
	FRotator FutureRot = UKismetMathLibrary::MakeRotFromX(FutureVelocity);
	FRotator CurrentRot = UKismetMathLibrary::MakeRotFromX(Velocity);
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(FutureRot, CurrentRot);

	float AbsYawDelta = FMath::Abs(DeltaRot.Yaw);
	
	float RotationValue = 0.0f;
	switch (RotationMode)
	{
	case E_RotationMode::OrientToMovement:
		RotationValue = 60.0f;
		break;
	case E_RotationMode::Strafe:
		RotationValue = 40.0f;
		break;
	default:
		RotationValue = 0.0f;
		break;
	}

	if (AbsYawDelta >= RotationValue)
	{
		return true;
	}
	return false;
	
}

bool UTFAnimInstance::ShouldTurnInPlace()
{
	float RootYawDelta = FMath::Abs(
	UKismetMathLibrary::NormalizedDeltaRotator(
		CharacterTransform.GetRotation().Rotator(),
		RootTransform.GetRotation().Rotator()
	).Yaw);

	// bool bisStop = false;
	// if (MovementState == E_MovementState::Idle && MovementStateLastFrame == E_MovementState::Moving)
	// {
	// 	bisStop = true;
	// }
	//
	// bool bTurnInPlace = false;
	// if (RootYawDelta >= 50.0f && bisStop)
	// {
	// 	bTurnInPlace = true;
	// }
	
	bool bTurnInPlace = false;
	if (RootYawDelta >= 50.0f)
	{
		bTurnInPlace = true;
	}
	
	if (GEngine)
	{
		// if (bTurnInPlace)	
		// 	GEngine->AddOnScreenDebugMessage(39, 1, FColor::Blue, "Turn In Place True");
		// else
		// {
		// 	GEngine->AddOnScreenDebugMessage(39, 1, FColor::Blue, "Turn In Place False");
		// }
	}

	// GEngine->AddOnScreenDebugMessage(41, 1, FColor::Cyan,
	// FString::Printf(TEXT("RootYawDelta: %.2f"), RootYawDelta));
	
	return bTurnInPlace;
}


bool UTFAnimInstance::ShouldSpinTransition()
{
	float RootYawDelta = FMath::Abs(
	UKismetMathLibrary::NormalizedDeltaRotator(
		CharacterTransform.GetRotation().Rotator(),
		RootTransform.GetRotation().Rotator()
	).Yaw);
	bool bisTurn = false;
	if (RootYawDelta >= 130.0f)
	{
		bisTurn = true;
	}
	

	bool bisMoving = false;
	if (Speed2D >= 150.0f)
	{
		bisMoving = true;
	}

	bool bHasPivotTag = !CurrentDatabaseTags.Contains("Pivots");

	return bisTurn && bisMoving && bHasPivotTag;
}

bool UTFAnimInstance::JustLandedLight()
{
	if (TFPlayerCharacter)
	{
		bool bJustLanded = false;
		if (TFPlayerCharacter->GetJustLanded())
		{
			bJustLanded = true;
		}

		bool bLandSpeed = false;
		float LandVelocity_Z = FMath::Abs(TFPlayerCharacter->GetLandVelocity().Z);
		float HeayLandSpeed = FMath::Abs(HeayLandSpeedThreshold);
		if (LandVelocity_Z < HeayLandSpeed)
		{
			bLandSpeed = true;
		}

		return bJustLanded && bLandSpeed;
	}
	return false;
}

bool UTFAnimInstance::JustLandedHeavy()
{
	if (TFPlayerCharacter)
	{
		bool bJustLanded = false;
		if (TFPlayerCharacter->GetJustLanded())
		{
			bJustLanded = true;
		}

		bool bLandSpeed = false;
		float LandVelocity_Z = FMath::Abs(TFPlayerCharacter->GetLandVelocity().Z);
		float HeayLandSpeed = FMath::Abs(HeayLandSpeedThreshold);
		if (LandVelocity_Z >= HeayLandSpeed)
		{
			bLandSpeed = true;
		}

		return bJustLanded && bLandSpeed;
	}
	return false;
}
