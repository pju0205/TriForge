// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/DSPlayerController.h"
#include "TFPlayerController.generated.h"

class ATFHUD;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuitMenuOpen, bool, bOpen);		// Server
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

	UPROPERTY(EditAnywhere, Category="Input") // Quit 버튼 설정
	TObjectPtr<UInputAction> QuitAction;

	UPROPERTY(EditAnywhere, Category="Input") 
	TObjectPtr<UInputAction> AimAction;

	UPROPERTY(EditAnywhere, Category="Input") 
	TObjectPtr<UInputAction> EquipAction;

	UPROPERTY(EditAnywhere, Category="Input") 
	TObjectPtr<UInputAction> AttackAction;

	UPROPERTY(EditAnywhere, Category="Input") 
	TObjectPtr<UInputAction> TestAction;
	
	FVector2d MoveDir = FVector2d::ZeroVector;
	
	void Move(const struct FInputActionValue& InputActionValue);
	void Rotation(const struct FInputActionValue& InputActionValue);
	// void CrouchStart(const struct FInputActionValue& InputActionValue);
	// void CrouchEnd(const struct FInputActionValue& InputActionValue);
	void SprintStart(const struct FInputActionValue& InputActionValue);
	void SprintEnd(const struct FInputActionValue& InputActionValue);
	void Jump(const struct FInputActionValue& InputActionValue);
	void Slide(const struct FInputActionValue& InputActionValue);

	// Quit 버튼 관련
	void Input_Quit();		// Server
	bool bQuitMenuOpen;		// Server

	void AimingStarted(const struct FInputActionValue& AimActionValue);
	void AimingReleased(const struct FInputActionValue& AimActionValue);
	void EquipWeapon(const struct FInputActionValue& InputActionValue);
	void WeaponAttackStarted(const struct FInputActionValue& InputActionValue);
	void WeaponAttackReleased(const struct FInputActionValue& InputActionValue);
	void TestMontagePlay(const struct FInputActionValue& InputActionValue);
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

public:
	ATFPlayerController();
	virtual void Tick(float DeltaTime) override;

	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDAmmo(int32 Ammo);

	virtual void OnPossess(APawn* InPawn) override;
	
	UPROPERTY(BlueprintAssignable)		// Quit 버튼 설정
	FOnQuitMenuOpen OnQuitMenuOpen;

private:
	UPROPERTY()
	ATFHUD* TFHUD;
};
