
#pragma once

#include "CoreMinimal.h"
#include "Weapon/TFWeapon.h"
#include "TFRangedWeapon.generated.h"

class ATFWeaponPlayerController;
class ATFWeaponCharacter;
class ATFProjectile;
/**
 * 
 */
UCLASS()
class TRIFORGE_API ATFRangedWeapon : public ATFWeapon
{
	GENERATED_BODY()

protected:

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<ATFProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Ammo)
	int32 Ammo;

	UFUNCTION()
	void OnRep_Ammo();

	void SpendAmmo();
	
	UPROPERTY(EditAnywhere)
	int32 MagCapacity;

	UPROPERTY()
	ATFWeaponCharacter* TFOwnerCharacter;
	
	UPROPERTY()
	ATFWeaponPlayerController* TFOwnerController;
	
public:
	ATFRangedWeapon();
	
	virtual void Attack(const FVector& HitTarget) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void OnRep_Owner() override;

	virtual void Dropped() override;

	void SetHUDRangedWeaponAmmo();

	bool IsAmmoEmpty();
};
