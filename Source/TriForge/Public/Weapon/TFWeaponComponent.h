
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TFWeaponComponent.generated.h"


class ATFWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TRIFORGE_API UTFWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTFWeaponComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	friend class ATFMyCharacter;

	void EquipWeapon(ATFWeapon* WeaponToEquip);

	void FireButtonPressed(bool bPressed);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	ATFMyCharacter* Character;

	UPROPERTY(Replicated)
	ATFWeapon* EquippedWeapon;

	bool bFireButtonPressed;
	
};
