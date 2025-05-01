// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/TFRangedWeapon.h"

#include "Character/TFWeaponCharacter.h"
#include "Character/TFWeaponPlayerController.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/TFProjectile.h"


ATFRangedWeapon::ATFRangedWeapon()
{
	SetWeaponClass(EWeaponClass::Ewc_RangedWeapon);
}

void ATFRangedWeapon::Attack(const FVector& HitTarget, const FHitResult& HitResult)
{
	Super::Attack(HitTarget, HitResult);

	//if (!HasAuthority()) return;

	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if (MuzzleFlashSocket)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());

		// muzzle flash 소켓에서 hit location 까지의 백터
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();
		
		if (ProjectileClass && InstigatorPawn)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GetOwner();
			SpawnParams.Instigator = InstigatorPawn;
			UWorld* World = GetWorld();
			if (World)
			{
				World->SpawnActor<ATFProjectile>(
					ProjectileClass,
					SocketTransform.GetLocation(),
					TargetRotation,
					SpawnParams
				);
			}
		}
	}

	SpendAmmo();
}

void ATFRangedWeapon::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATFRangedWeapon, Ammo);
}

void ATFRangedWeapon::SetHUDRangedWeaponAmmo()
{
	TFOwnerCharacter = TFOwnerCharacter == nullptr ? Cast<ATFWeaponCharacter>(GetOwner()) : TFOwnerCharacter;
	if (TFOwnerCharacter)
	{
		TFOwnerController = TFOwnerController == nullptr ? Cast<ATFWeaponPlayerController>(TFOwnerCharacter->GetController()) : TFOwnerController;

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

