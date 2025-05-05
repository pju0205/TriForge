// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/TFWeapon.h"
#include "TFMeleeWeapon.generated.h"

class UBoxComponent;
/**
 * 
 */
UCLASS()
class TRIFORGE_API ATFMeleeWeapon : public ATFWeapon
{
	GENERATED_BODY()

public:
	ATFMeleeWeapon();

	virtual void Attack(const FHitResult& HitResult, const FVector& SocketLocation) override;

	UPROPERTY(EditAnywhere)
	UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere)
	float Damage = 10.f;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
