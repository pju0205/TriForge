

#include "Weapon/TFWeaponComponent.h"

#include "TFMyCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/TFWeapon.h"

UTFWeaponComponent::UTFWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
}

void UTFWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void UTFWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

void UTFWeaponComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTFWeaponComponent, EquippedWeapon);
}

void UTFWeaponComponent::EquipWeapon(ATFWeapon* WeaponToEquip)
{
	// Character == nullptr OR OverlappingWeapon == nullptr
	if (Character == nullptr || WeaponToEquip == nullptr) return;
	
	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::Ews_Equipped);

	const USkeletalMeshSocket* WeaponSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (WeaponSocket)
	{
		WeaponSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}
	
	EquippedWeapon->SetOwner(Character);
}

void UTFWeaponComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;
	if (bFireButtonPressed)
	{
		
	}
}
