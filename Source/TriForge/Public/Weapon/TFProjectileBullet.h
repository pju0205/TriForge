// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/TFProjectile.h"
#include "TFProjectileBullet.generated.h"

/**
 * 
 */
UCLASS()
class TRIFORGE_API ATFProjectileBullet : public ATFProjectile
{
	GENERATED_BODY()

protected:
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	
};
