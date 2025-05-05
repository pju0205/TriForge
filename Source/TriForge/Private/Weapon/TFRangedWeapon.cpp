// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/TFRangedWeapon.h"

#include "Character/TFWeaponCharacter.h"
#include "Character/TFWeaponPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/TFProjectile.h"


ATFRangedWeapon::ATFRangedWeapon()
{
	SetWeaponClass(EWeaponClass::Ewc_RangedWeapon);
}

void ATFRangedWeapon::Attack_Implementation(const FHitResult& HitResult, const FVector& SocketLocation)
{
	Super::Attack(HitResult, SocketLocation);

	//if (!HasAuthority()) return;
	// 디버깅용 코드
	/*if (GEngine)
	{
		FVector aa = HitResult.ImpactPoint;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
			FString::Printf(TEXT("%f, %f, %f"), aa.X, aa.Y, aa.Z));
		GEngine->AddOnScreenDebugMessage(-1,5.f, FColor::Cyan,
			FString::Printf(TEXT("%f, %f, %f"),HitResult.TraceEnd.X, HitResult.TraceEnd.Y, HitResult.TraceEnd.Z));
	}
	DrawDebugSphere(GetWorld(), SocketLocation, 12.f, 5, FColor::Green, false);
	DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 12.f, 5, FColor::Blue, false);
	DrawDebugLine(GetWorld(), SocketLocation, HitResult.ImpactPoint, FColor::Red, false, 3.f);
	DrawDebugLine(GetWorld(), SocketLocation, HitResult.TraceEnd, FColor::Yellow, false, 3.f);*/
	
	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	if (HitResult.bBlockingHit)
	{
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
					SocketLocation,
					(HitResult.ImpactPoint-SocketLocation).Rotation(),
					SpawnParams
				);
			}
		}
	}
	else
	{
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
					SocketLocation,
					(HitResult.TraceEnd - SocketLocation).Rotation(),
					SpawnParams
				);
			}
		}
	}
	AttackEffects();

	SpendAmmo();
}

void ATFRangedWeapon::AttackEffects_Implementation()
{
	PlayAttackMontage();
	GetWeaponMesh()->PlayAnimation(RangedWeaponAnimation, false);
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

