
#pragma once

#include "CoreMinimal.h"
#include "PlayerState/TFMatchPlayerState.h"
#include "TFPlayerState.generated.h"

class ATFPlayerController;
class ATFWeaponCharacter;
class ATFWeaponPlayerController;
/**
 * 
 */
UCLASS()
class TRIFORGE_API ATFPlayerState : public ATFMatchPlayerState
{
	GENERATED_BODY()

public:
	ATFPlayerState();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()
	void OnRep_Health();

	void CalcDamage(float Damage);

	FORCEINLINE float GetCurrentHealth() const {return CurrentHealth; }
	FORCEINLINE float GetMaxHealth() const {return MaxHealth; }

private:
	UPROPERTY()
	ATFPlayerController* TFPlayerController;
	
	UPROPERTY(EditAnywhere, Category="PlayerState")
	float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing=OnRep_Health, VisibleAnywhere, Category="PlayerState")
	float CurrentHealth = 100.f;
};
