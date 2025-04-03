#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TFMyCharacter.generated.h"

class UTFWeaponComponent;
class ATFWeapon;
class USpringArmComponent;
class UCameraComponent;
class UTFMyAnimInstance;
class UCharacterTrajectoryComponent;

UCLASS()
class TRIFORGE_API ATFMyCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowedClasses = "true"))
	TObjectPtr<UCharacterTrajectoryComponent>CharacterTrajectory = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowedClasses = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom = nullptr;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowedClasses = "true"))
	TObjectPtr<UCameraComponent> Camera = nullptr;*/

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	UTFMyAnimInstance* MyAnimInstance;

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float DefaultSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed;

public:	
	bool IsSprinting = false;
	bool IsCrouching = false;

	ATFMyCharacter();
	virtual void Tick(float DeltaTime) override;
	
	void UpdateSprintState(bool bIsSprint);
	void UpdateCrouchState(bool bIsCrouch);

	TObjectPtr<UCharacterTrajectoryComponent> GetCharacterTrajectoryComponent() { return CharacterTrajectory; };
	bool GetIsCrouching() { return IsCrouching; };


//
// Weapon 코드 시작
//
protected:
public:
	virtual void PostInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	void SetOverlappingWeapon(ATFWeapon* Weapon);

	bool bIsWeaponEquipped();

	void EquipButtonPressed();

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastEquipButtonPressed();
	
	void FireButtonPressed();
	void FireButtonReleased();

private:
	UPROPERTY(VisibleAnywhere)
	UTFWeaponComponent* WeaponComponent;
	
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	ATFWeapon* OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(ATFWeapon* LastWeapon);
};
