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
	void AttackEffects();

	UFUNCTION(NetMulticast, Reliable)
	void ImpactEffects(const FHitResult& Hit);

	UFUNCTION(NetMulticast, Reliable)
	void BeamEffects(const FHitResult& Hit);

private:
	UPROPERTY(EditAnywhere)
	float Damage = 20.f;

	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere)
	UParticleSystem* BeamParticles;

	UPROPERTY(EditAnywhere)
	UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere)
	USoundCue* FireSound;

	UPROPERTY(EditAnywhere)
	USoundCue* ImpactSound;
};
