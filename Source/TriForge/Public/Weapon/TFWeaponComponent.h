
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

	void SetAiming(bool bIsAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION(Client, Reliable)
	void ClientResetAiming();

	// 들고 있는 무기 삭제 함수 (태영 추가)
	void DropWeapon();

	UPROPERTY(Transient, Replicated)
	TArray<ATFWeapon*> Inventory;
	

protected:
	virtual void BeginPlay() override;

	
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

	// when not aiming view
	// FOV = Field Of View
	float DefaultFOV;

	UPROPERTY(EditAnywhere, Category = "Settings")
	float ZoomedFOV = 30.f;

	float CurrentFOV;

	UPROPERTY(EditAnywhere, Category = "Settings")
	float ZoomInterpSpeed = 20.f;

	void InterpFOV(float DeltaTime);

	UPROPERTY()
	FVector2D RecoilOffset;

	float RecoilYawBias = 0.f;
	void ApplyRecoil(float DeltaTime);

	
	void InitializeVariables();
};
