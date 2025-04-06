
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TFWeaponPlayerController.generated.h"

class UInputAction;
class UInputMappingContext;
/**
 * 
 */
UCLASS()
class TRIFORGE_API ATFWeaponPlayerController : public APlayerController
{
	GENERATED_BODY()


private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> CharacterContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> RotationAction;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;

	/*UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> SprintAction;*/

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> EquipAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> AttackAction;

	
	void Move(const struct FInputActionValue& InputActionValue);
	void Rotation(const struct FInputActionValue& InputActionValue);
	void Jump(const struct FInputActionValue& InputActionValue);
	//void Sprint(const struct FInputActionValue& InputActionValue);

	void EquipWeapon(const struct FInputActionValue& InputActionValue);
	void WeaponAttackStarted(const struct FInputActionValue& InputActionValue);
	void WeaponAttackReleased(const struct FInputActionValue& InputActionValue);

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

public:
	ATFWeaponPlayerController();
	virtual void Tick(float DeltaTime) override;
	
};
