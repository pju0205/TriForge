// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/DSPlayerController.h"
#include "TFPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class TRIFORGE_API ATFPlayerController : public ADSPlayerController
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputMappingContext> TFCharacterContext;

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> RotationAction;

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> SlideAction; 

	FVector2d MoveDir = FVector2d::ZeroVector;
	
	void Move(const struct FInputActionValue& InputActionValue);
	void Rotation(const struct FInputActionValue& InputActionValue);
	// void CrouchStart(const struct FInputActionValue& InputActionValue);
	// void CrouchEnd(const struct FInputActionValue& InputActionValue);
	void SprintStart(const struct FInputActionValue& InputActionValue);
	void SprintEnd(const struct FInputActionValue& InputActionValue);
	void Jump(const struct FInputActionValue& InputActionValue);
	void Slide(const struct FInputActionValue& InputActionValue);

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

public:
	ATFPlayerController();
	virtual void Tick(float DeltaTime) override;
};
