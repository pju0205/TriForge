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
	TObjectPtr<UInputAction> MoveAction; // �̵�

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> RotationAction; // ȸ��

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> JumpAction; // ����

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> SprintAction; // �޸���

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> CrouchAction; // �ɱ�

	void Move(const struct FInputActionValue& InputActionValue); // �̵�
	void Rotation(const struct FInputActionValue& InputActionValue); // ȸ��
	void CrouchStart(const struct FInputActionValue& InputActionValue); // �޸���
	void CrouchEnd(const struct FInputActionValue& InputActionValue); // �޸���
	void SprintStart(const struct FInputActionValue& InputActionValue); // �޸���
	void SprintEnd(const struct FInputActionValue& InputActionValue); // �޸���
	void Jump(const struct FInputActionValue& InputActionValue); // ����

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

public:
	ATFMyPlayerController();
	virtual void Tick(float DeltaTime) override;
};
