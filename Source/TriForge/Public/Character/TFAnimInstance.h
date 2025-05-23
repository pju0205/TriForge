// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimNodeReference.h"
#include "PoseSearch/PoseSearchTrajectoryLibrary.h"
#include "Character/TFPlayerCharacter.h"
#include "TFAnimInstance.generated.h"

enum class EWeaponType : uint8;
class ATFPlayerCharacter;
class UCharacterMovementComponent;
class UPoseSearchDatabase;

UENUM(BlueprintType)
enum class E_MovementMode : uint8
{
	OnGorund	UMETA(DisplayName = "OnGorund"),
	InAir	UMETA(DisplayName = "InAir")
};

UENUM(BlueprintType)
enum class E_RotationMode : uint8
{
	OrientToMovement	UMETA(DisplayName = "OrientToMovement"),
	Strafe	UMETA(DisplayName = "Strafe")
};

UENUM(BlueprintType)
enum class E_MovementState : uint8
{
	Idle	UMETA(DisplayName = "Idle"),
	Moving	UMETA(DisplayName = "Moving")
};

// 근거리 무기이냐 원거리 무기이냐 OR 라이플이냐, 샷건이냐, 칼이냐, 헤머냐
// 전자는 무기가 무엇인지 상관없이 원거리일 때 애니메이션 1개, 근거리 일 때 애니메이션 1개로 공용으로 돌려쓰기
// 후자는 같은 원거리 무기, 근거리 무기에 상관없이 무기 하나 하나 별로 다른 애니메이션 사용
UENUM(BlueprintType)
enum class E_EquippedWeaponType : uint8
{
	Rifle UMETA(DisplayName = "Rifle"),
	ShotGun UMETA(DisplayName = "ShotGun"),
	Pistol UMETA(DisplayName = "Pistol"),
	Knife UMETA(DisplayName = "Knife"),
	Hammer UMETA(DisplayName = "Hammer"),
	UnEquipped UMETA(DisplayName = "UnEquipped")
};

USTRUCT(BlueprintType)
struct FChooserTableIn
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bisStarting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bisPivoting;
};


UCLASS()
class TRIFORGE_API UTFAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
private:
	float HeayLandSpeedThreshold;
	
	void SetRootTransform();
	void SetAcceleration();
	void SetVelocity();
	void UpdateEssentialValues();

	void GenerateTrajectory(float DeltaTime);

	void UpdateStates();
	
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	ATFPlayerCharacter* TFPlayerCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	UCharacterMovementComponent* TFCharacterMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EssentialValue")
	FTransform CharacterTransform;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EssentialValue")
	bool OffsetRootBoneEnabled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EssentialValue")
	FTransform RootTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EssentialValue")
	FVector Acceleration;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EssentialValue")
	float AccelerationAmount;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EssentialValue")
	bool bHasAcceleration;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EssentialValue")
	FVector Velocity;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EssentialValue")
	FVector VelocityLastFrame;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EssentialValue")
	FVector VelocityAcceleration;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EssentialValue")
	FVector LastNonZeroVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EssentialValue")
	float Speed2D;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EssentialValue")
	bool bHasVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EssentialValue")
	UPoseSearchDatabase* CurrentSelectedDatabase;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EssentialValue")
	TArray<FName> CurrentDatabaseTags;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PoseSearch")
	FPoseSearchTrajectoryData TrajectoryGenerationDataIdle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PoseSearch")
	FPoseSearchTrajectoryData TrajectoryGenerationDataMoving;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PoseSearch")
	FPoseSearchQueryTrajectory Trajectory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PoseSearch")
	FPoseSearchTrajectory_WorldCollisionResults TrajectoryCollision;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PoseSearch")
	float PreviousDesiredControllerYaw;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PoseSearch")
	FVector FutureVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "States")
	E_MovementMode MovementMode;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "States")
	E_MovementMode MovementModeLastFrame;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "States")
	E_RotationMode RotationMode;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "States")
	E_RotationMode RotationModeLastFrame;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "States")
	E_MovementState MovementState;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "States")
	E_MovementState MovementStateLastFrame;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "States")
	E_EquippedWeaponType WeaponTypeState;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "States")
	E_Gait Gait;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "States")
	E_Gait GaitLastFrame;
	
	UFUNCTION(BlueprintPure, Category = "MomvementAnlaysis", meta = (BlueprintThreadSafe))
	bool isMoving();
	
	UFUNCTION(BlueprintPure, Category = "MomvementAnlaysis", meta = (BlueprintThreadSafe))
	bool isStarting();

	UFUNCTION(BlueprintPure, Category = "MomvementAnlaysis", meta = (BlueprintThreadSafe))
	bool isPivoting();

	UFUNCTION(BlueprintPure, Category = "MomvementAnlaysis", meta = (BlueprintThreadSafe))
	bool ShouldTurnInPlace();

	UFUNCTION(BlueprintPure, Category = "MomvementAnlaysis", meta = (BlueprintThreadSafe))
	bool ShouldSpinTransition();

	UFUNCTION(BlueprintPure, Category = "MomvementAnlaysis", meta = (BlueprintThreadSafe))
	bool JustLandedLight();
	
	UFUNCTION(BlueprintPure, Category = "MomvementAnlaysis", meta = (BlueprintThreadSafe))
	bool JustLandedHeavy();
	
public:
	UTFAnimInstance();

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "EssentialValue")
	void SetOffsetRootNode();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EssentialValue")
	FAnimNodeReference OffsetRootNode;

	E_EquippedWeaponType CheckWeaponType(EWeaponType CurrentWeaponType);

	UPROPERTY(BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	bool bWeaponEquipped;
	
	UPROPERTY(BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	bool bRangedWeapon;
	
	UPROPERTY()
	ATFWeapon* EquippedWeapon;

private:
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	FTransform LeftHandTransform;
	
};
