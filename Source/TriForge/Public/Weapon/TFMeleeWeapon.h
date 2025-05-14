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

	UFUNCTION()
	virtual void Attack() override;

	UFUNCTION(Server, Reliable)
	void ServerAttack(const FHitResult& HitResult);

	UFUNCTION(Server, Reliable)
	void ServerAttackEffects();

	UFUNCTION(NetMulticast,Reliable)
	void MultiAttackEffects();
	
	UPROPERTY(EditAnywhere)
	UBoxComponent* CollisionBox;

	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UPROPERTY(EditAnywhere)
	float Damage = 10.f;

protected:
	virtual void BeginPlay() override;
	
	virtual void OnRep_WeaponState() override;
private:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* CollisionBoxStart;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* CollisionBoxEnd;

};
