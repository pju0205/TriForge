// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/TFCharacter.h"
#include "TFPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UCurveFloat;
// class UTFAnimInstance;

UENUM(BlueprintType)
enum class E_Gait : uint8
{
	Walk	UMETA(DisplayName = "Walk"),
	Sprint	UMETA(DisplayName = "Sprint")
};

UCLASS()
class TRIFORGE_API ATFPlayerCharacter : public ATFCharacter
{
	GENERATED_BODY()

private:
	void GetDesiredGait();
	float CalculateMaxSpeed(float& StrafeSpeedMap);
	void UpdateMovement();
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowedClasses = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowedClasses = "true"))
	TObjectPtr<UCameraComponent> Camera = nullptr;

	// UPROPERTY(BlueprintReadOnly, Category = "Animation")
	// UTFAnimInstance* TFAnimInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	E_Gait ECurrentGait;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FVector WalkSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FVector SprintSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bSprinting;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bWalking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	UCurveFloat* StrafeSpeedMapCurve;
	
public:
	ATFPlayerCharacter();
	
	virtual void Tick(float DeltaTime) override;

	E_Gait GetGait() const {return ECurrentGait;};
};
