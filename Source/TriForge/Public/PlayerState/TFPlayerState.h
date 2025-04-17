
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TFPlayerState.generated.h"

class ATFWeaponCharacter;
class ATFWeaponPlayerController;
/**
 * 
 */
UCLASS()
class TRIFORGE_API ATFPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ATFPlayerState();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, Category="PlayerState")
	float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing=OnRep_Health, VisibleAnywhere, Category="PlayerState")
	float CurrentHealth = 100.f;

	UFUNCTION()
	void OnRep_Health();

	void CalcDamage(float Damage);


private:
	UPROPERTY()
	ATFWeaponCharacter* TFCharacter;
	
	UPROPERTY()
	ATFWeaponPlayerController* TFPlayerController;
	
};
