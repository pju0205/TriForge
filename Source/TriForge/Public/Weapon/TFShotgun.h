// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/TFHitScanWeapon.h"
#include "TFShotgun.generated.h"

/**
 * 
 */
UCLASS()
class TRIFORGE_API ATFShotgun : public ATFHitScanWeapon
{
	GENERATED_BODY()

public:
	ATFShotgun();
	virtual void Attack() override;

	UFUNCTION(Server, Reliable)
	void ServerShotgunAttack(ATFPlayerCharacter* DamagedCharacter, float TotalDamage);

	UFUNCTION(Server, Reliable)
	void ServerAttackEffects();

	UFUNCTION(Server, Reliable)
	void ServerImpactEffect(const FHitResult& Hit);
	
	UFUNCTION(Server, Reliable)
	void ServerBeamEffect(const FHitResult& Hit);
	
private:
	// 산탄으로 나갈 탄알 개수
	UPROPERTY(EditAnywhere, Category = "Settings")
	uint32 NumberOfFrangible = 35;
	
	void CountingHit(const FVector& Start, const FVector& End);

	UPROPERTY()
	TMap<ATFPlayerCharacter* , uint32> HitMap;

	UPROPERTY()
	TMap<ATFPlayerCharacter* , uint32> HeadShotHitMap;

};
