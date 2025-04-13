
#pragma once

#include "CoreMinimal.h"
#include "Weapon/TFWeapon.h"
#include "TFRangedWeapon.generated.h"

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

public:
	ATFRangedWeapon();
	virtual void Attack(const FVector& HitTarget) override;

};
