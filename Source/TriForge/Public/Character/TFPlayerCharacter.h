// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/TFCharacter.h"
#include "TFPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UCurveFloat;
class UAnimMontage;
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
	
	virtual void Landed(const FHitResult& Hit) override;
	UFUNCTION()
	void OnDelayComplete();

	void SetSlideMontage(bool bisSlideDir);
	
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
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bSprinting;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bWalking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	UCurveFloat* StrafeSpeedMapCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
	bool bJustLanded;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
	FVector LandVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* SlideMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* ForwardSlide_Montage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* BackSlide_Montage; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* RightSlide_Montage; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* LeftSlide_Montage; 
	
public:
	ATFPlayerCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	void UpdateSprintState(bool isSprint);
	UFUNCTION(Server, Reliable)
	void ServerUpdateSprintState(bool isSprint);
	
	void SetSlideDir(float Forward, float Right); 
	void isPlayingSlideMontage(float Forward, float Right); 
	void PlaySlidMontage();
	
	E_Gait GetGait() const {return ECurrentGait;};
	bool GetJustLanded() const {return bJustLanded;};
	FVector GetLandVelocity() {return LandVelocity;};
	
};

