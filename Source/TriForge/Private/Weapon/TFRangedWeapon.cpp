// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/TFRangedWeapon.h"

#include "Character/TFPlayerCharacter.h"
#include "Character/TFPlayerController.h"
#include "Net/UnrealNetwork.h"



ATFRangedWeapon::ATFRangedWeapon()
{
	SetWeaponClass(EWeaponClass::Ewc_RangedWeapon);
}

void ATFRangedWeapon::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATFRangedWeapon, Ammo);
}

void ATFRangedWeapon::SetHUDRangedWeaponAmmo()
{
	TFOwnerCharacter = TFOwnerCharacter == nullptr ? Cast<ATFPlayerCharacter>(GetOwner()) : TFOwnerCharacter;
	if (TFOwnerCharacter)
	{
		TFOwnerController = TFOwnerController == nullptr ? Cast<ATFPlayerController>(TFOwnerCharacter->GetController()) : TFOwnerController;

		if (TFOwnerController)
		{
			TFOwnerController->SetHUDAmmo(Ammo);
		}
	}
}

bool ATFRangedWeapon::IsAmmoEmpty()
{
	return Ammo <= 0;
}

void ATFRangedWeapon::SpendAmmo()
{
	Ammo = FMath::Clamp(Ammo - 1 , 0, MagCapacity);

	SetHUDRangedWeaponAmmo();
}

void ATFRangedWeapon::OnRep_Ammo()
{
	SetHUDRangedWeaponAmmo();
}

void ATFRangedWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();
	if (Owner == nullptr)
	{
		TFOwnerCharacter = nullptr;
		TFOwnerController = nullptr;
	}
	else
	{
		SetHUDRangedWeaponAmmo();
	}

}

void ATFRangedWeapon::Dropped()
{
	TFOwnerCharacter = nullptr;
	TFOwnerController = nullptr;

	Super::Dropped();
}

