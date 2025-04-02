// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TFMyAnimInstance.generated.h"

class ATFMyCharacter;
class UCharacterMovementComponent;
class UCharacterTrajectoryComponent;

UCLASS()
class TRIFORGE_API UTFMyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly)
	ATFMyCharacter* MyCharacter;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	UCharacterMovementComponent* MyCharacterMovement;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UCharacterTrajectoryComponent>CharacterTrajectory = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float Direction;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool IsJump;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool IsCrouching;

public:
	UTFMyAnimInstance();

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;
};
