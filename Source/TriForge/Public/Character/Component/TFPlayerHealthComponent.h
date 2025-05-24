// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TFPlayerHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHealth_DeathEvent, AActor*, OwningActor, AActor*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHealth_AttributeChanged, float, CurrentHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthCompReplicated);

UENUM(BlueprintType)
enum class EDeathState : uint8
{
	NotDead = 0,
	DeathStarted,
	DeathFinished
};

// Player Health 관련 모든 처리하는 Component
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TRIFORGE_API UTFPlayerHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTFPlayerHealthComponent();

	// 데미지 처리 함수
	UFUNCTION(BlueprintCallable)
	virtual void CalcDamage(float Amount, AActor* Instigator);

	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	// Begins the death sequence for the owner.
	virtual void StartDeath(AActor* Instigator);

	// Ends the death sequence for the owner.
	virtual void FinishDeath(AActor* Instigator);

	UPROPERTY(ReplicatedUsing=OnRep_DeathState)
	EDeathState DeathState;

	UPROPERTY(BlueprintAssignable)
	FOnHealthCompReplicated OnHealthCompReplicated;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(ReplicatedUsing=OnRep_Health)
	float CurrentHealth = 100;

	UPROPERTY(ReplicatedUsing=OnRep_MaxHealth)
	float MaxHealth = 100;

	UFUNCTION()
	virtual void OnRep_DeathState(EDeathState OldDeathState);
	
	UFUNCTION()
	void OnRep_Health();

	UFUNCTION()
	void OnRep_MaxHealth();

public:

	UPROPERTY(BlueprintAssignable)
	FHealth_AttributeChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FHealth_AttributeChanged OnMaxHealthChanged;
	
	UPROPERTY(BlueprintAssignable)
	FHealth_DeathEvent OnDeathStarted;
	
	UPROPERTY(BlueprintAssignable)
	FHealth_DeathEvent OnDeathFinished;
	
};
