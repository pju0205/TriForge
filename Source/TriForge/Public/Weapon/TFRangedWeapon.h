
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
	ATFPlayerCharacter* TFOwnerCharacter;
	
	UPROPERTY()
	ATFPlayerController* TFOwnerController;

	
public:
	ATFRangedWeapon();
	
	UFUNCTION()
	virtual void Attack() override;

	UFUNCTION(Server, Reliable)
	void ServerAttack(const FHitResult& HitResult, const FVector& SocketLocation);
	
	UFUNCTION(NetMulticast, Reliable)
	void AttackEffects();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void OnRep_Owner() override;

	virtual void Dropped() override;

	void SetHUDRangedWeaponAmmo();

	bool IsAmmoEmpty();

	void TraceEnemy(FHitResult& TraceHitResult, float TraceLength);
};
