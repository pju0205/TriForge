// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/TFWeapon.h"
#include "TFMeleeWeapon.generated.h"

/**
 * 
 */
UCLASS()
class TRIFORGE_API ATFMeleeWeapon : public ATFWeapon
{
	GENERATED_BODY()

public:
	ATFMeleeWeapon();

	virtual void Attack(const FVector& HitTarget) override;
};
