
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TFWeaponCharacter.generated.h"

class ATFPlayerState;
class ATFWeaponPlayerController;
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

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:


public:
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* FollowCamera;
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;
	virtual void PostInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void SetOverlappingWeapon(ATFWeapon* Weapon);

	bool IsWeaponEquipped();
	bool IsAiming();

	void AimButtonPressed();
	void AimButtonReleased();
	void EquipButtonPressed();
	void AttackButtonPressed();
	void AttackButtonReleased();
	
	UTFWeaponComponent* GetWeaponComponent();


private:
	UPROPERTY(VisibleAnywhere)
	UTFWeaponComponent* WeaponComponent;

	UPROPERTY(ReplicatedUsing= OnRep_OverlappingWeapon)
	ATFWeapon* OverlappingWeapon;

	UPROPERTY()
	ATFWeaponPlayerController* TFPlayerController;
	UPROPERTY()
	ATFPlayerState* TFPlayerState;

	UFUNCTION()
	void OnRep_OverlappingWeapon(ATFWeapon* LastWeapon);

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastEquipButtonPressed();
	
};
