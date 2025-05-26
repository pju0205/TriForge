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
	float Damage = 10.f;

	UFUNCTION()
	void BeginTrace();
	UFUNCTION()
	void EndTrace();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	USceneComponent* TraceStart;

	UPROPERTY(EditAnywhere)
	USceneComponent* TraceEnd;

	FTimerHandle TraceTimerHandle;

	UPROPERTY()
	TArray<AActor*> AlreadyHitActors;

	void StartTraceTimer();

	void StopTraceTimer();

	void TraceEnemy();

	FVector PreTraceStart;
	FVector PreTraceEnd;

};
