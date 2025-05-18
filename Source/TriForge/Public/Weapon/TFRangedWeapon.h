
#pragma once

#include "CoreMinimal.h"
#include "Weapon/TFWeapon.h"
#include "TFRangedWeapon.generated.h"

class ATFPlayerController;
class ATFPlayerCharacter;
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
	virtual void SpendAmmo();

	UPROPERTY()
	ATFPlayerCharacter* TFOwnerCharacter;
	
	UPROPERTY()
	ATFPlayerController* TFOwnerController;
	
private:
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Ammo)
	int32 Ammo;

	UFUNCTION()
	void OnRep_Ammo();
	
	UPROPERTY(EditAnywhere)
	int32 MagCapacity;

public:
	ATFRangedWeapon();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void OnRep_Owner() override;

	virtual void Dropped() override;

	void SetHUDRangedWeaponAmmo();

	bool IsAmmoEmpty();

};
