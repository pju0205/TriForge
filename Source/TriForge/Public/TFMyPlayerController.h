// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TFMyPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class TRIFORGE_API ATFMyPlayerController : public APlayerController
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputMappingContext> MyCharacterContext;

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> MoveAction; // 이동

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> RotationAction; // 회전

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> JumpAction; // 점프

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> SprintAction; // 달리기

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> CrouchAction; // 앉기

	// Weapon 관련 입력 처리
	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> EquipActoin;

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> FireAction;

	void Move(const struct FInputActionValue& InputActionValue); // 이동
	void Rotation(const struct FInputActionValue& InputActionValue); // 회전
	void CrouchStart(const struct FInputActionValue& InputActionValue); // 달리기
	void CrouchEnd(const struct FInputActionValue& InputActionValue); // 달리기
	void SprintStart(const struct FInputActionValue& InputActionValue); // 달리기
	void SprintEnd(const struct FInputActionValue& InputActionValue); // 달리기
	void Jump(const struct FInputActionValue& InputActionValue); // 점프

	// Weapon 관련 입력 함수
	void EquipWeapon(const struct FInputActionValue& InputActionValue); // 무기 장착
	/*UFUNCTION(Server, Reliable)
	void ServerEquipWeapon();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastEquipWeapon();*/

	void FireWeapon(const struct FInputActionValue& InputActionValue); // 무기 발사

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

public:
	ATFMyPlayerController();
	virtual void Tick(float DeltaTime) override;
};
