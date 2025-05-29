
#pragma once

#include "CoreMinimal.h"
#include "Weapon/TFRangedWeapon.h"
#include "TFProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class TRIFORGE_API ATFProjectileWeapon : public ATFRangedWeapon
{
	GENERATED_BODY()


public:
	ATFProjectileWeapon();
	
	UFUNCTION()
	virtual void Attack() override;

	UFUNCTION(Server, Reliable)
	void ServerAttack(const FHitResult& HitResult, const FVector& SocketLocation);

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<ATFProjectile> ProjectileClass;
	
	
};
