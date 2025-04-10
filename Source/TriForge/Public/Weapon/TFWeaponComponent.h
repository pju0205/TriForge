
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TFWeaponComponent.generated.h"

#define TRACE_LENGTH 80000.f

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

	UFUNCTION(Server, Reliable)
	void ServerAttackButton();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastAttackButton();
	
	ATFWeapon* GetEquippedWeapon();

	void TraceEnemy(FHitResult& TraceHitResult);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	ATFWeaponCharacter* Character;

	UPROPERTY(Replicated)
	ATFWeapon* EquippedWeapon;

	bool bAttackButtonPressed;

	FVector HitTarget;
	
};
