
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TFWeaponComponent.generated.h"

#define TRACE_LENGTH 80000.f

class ATFPlayerController;
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

	friend class ATFPlayerCharacter;
	
	void EquipWeapon(ATFWeapon* WeaponToEquip);

	void AttackButtonPressed(bool bPressed);
	
	ATFWeapon* GetEquippedWeapon();
	
	void SetHUDCrosshairs(float DeltaTime);

protected:
	virtual void BeginPlay() override;

	void SetAiming(bool bIsAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);
private:
	UPROPERTY()
	ATFPlayerCharacter* PlayerCharacter;

	UPROPERTY()
	ATFPlayerController* PlayerController;
	
	UPROPERTY()
	ATFHUD* HUD;

	UPROPERTY(Replicated)
	bool bAiming;

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
