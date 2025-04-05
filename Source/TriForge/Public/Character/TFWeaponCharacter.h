
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TFWeaponCharacter.generated.h"

class UTFWeaponComponent;
class ATFWeapon;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class TRIFORGE_API ATFWeaponCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATFWeaponCharacter();

protected:
	virtual void BeginPlay() override;


public:	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* FollowCamera;

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


private:
	UPROPERTY(VisibleAnywhere)
	UTFWeaponComponent* WeaponComponent;

	UPROPERTY(ReplicatedUsing= OnRep_OverlappingWeapon)
	ATFWeapon* OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(ATFWeapon* LastWeapon);
};
