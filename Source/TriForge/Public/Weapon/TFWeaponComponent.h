
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TFWeaponComponent.generated.h"

#define TRACE_LENGTH 80000.f

class ATFHUD;
class ATFWeaponPlayerController;
class ATFWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TRIFORGE_API UTFWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTFWeaponComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	friend class ATFWeaponCharacter;

	void EquipWeapon(ATFWeapon* WeaponToEquip);

	void AttackButtonPressed(bool bPressed);
	
	ATFWeapon* GetEquippedWeapon();

	void TraceEnemy(FHitResult& TraceHitResult);

	void SetHUDCrosshairs(float DeltaTime);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	ATFWeaponCharacter* Character;

	UPROPERTY()
	ATFWeaponPlayerController* Controller;

	UPROPERTY()
	ATFHUD* HUD;

	UPROPERTY(ReplicatedUsing=OnRep_EquippedWeapon)
	ATFWeapon* EquippedWeapon;

	UFUNCTION()
	void OnRep_EquippedWeapon();

	bool bAttackButtonPressed;
	
	FTimerHandle AttackTimer;
	
	bool bCanAttack = true;

	bool CanAttack();
	
	void StartAttackTimer();
	void AttackTimerFinished();

	void Attacking();
};
