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

	UFUNCTION(Server, Reliable)
	virtual void Attack(const FHitResult& HitResult, const FVector& SocketLocation) override;

	UPROPERTY(EditAnywhere)
	UBoxComponent* CollisionBox;

	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UPROPERTY(EditAnywhere)
	float Damage = 10.f;

	UFUNCTION(NetMulticast,Reliable)
	void AttackEffects();
protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	virtual void OnRep_WeaponState() override;
private:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* CollisionBoxStart;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* CollisionBoxEnd;


	UFUNCTION(NetMulticast, Reliable)
	void SetBoxCollision(bool bCollisionOn);
};
