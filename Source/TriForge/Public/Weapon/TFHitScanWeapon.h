// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/TFRangedWeapon.h"
#include "TFHitScanWeapon.generated.h"

/**
 * 
 */
UCLASS()
class TRIFORGE_API ATFHitScanWeapon : public ATFRangedWeapon
{
	GENERATED_BODY()

public:
	ATFHitScanWeapon();
	
	virtual void Attack() override;

	UFUNCTION(Server, Reliable)
	void ServerAttack(const FHitResult& Hit);
	
	UFUNCTION(NetMulticast, Reliable)
	void ImpactEffects(const FHitResult& Hit);

	UFUNCTION(NetMulticast, Reliable)
	void BeamEffects(const FHitResult& Hit);

	UPROPERTY(EditAnywhere, Category = "Settings")
	float Damage = 20.f;
	
	UPROPERTY(EditAnywhere, Category = "Settings")
	UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere, Category = "Settings")
	UParticleSystem* BeamParticles;

	UPROPERTY(EditAnywhere, Category = "Settings")
	UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere, Category = "Settings")
	USoundCue* FireSound;

	UPROPERTY(EditAnywhere, Category = "Settings")
	USoundCue* ImpactSound;
protected:
	FVector TraceWithScatter(const FVector& TraceStart, const FVector& HitTarget);

private:

	UPROPERTY(EditAnywhere, Category = "Settings")
	float DistanceToSphere = 800.f;
	
	UPROPERTY(EditAnywhere, Category = "Settings")
	float SphereRadius = 75.f;

	UPROPERTY(EditAnywhere, Category = "Settings")
	bool bIsScatter = false;
};
