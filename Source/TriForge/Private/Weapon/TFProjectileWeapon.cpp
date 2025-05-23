// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/TFProjectileWeapon.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Weapon/TFProjectile.h"

ATFProjectileWeapon::ATFProjectileWeapon()
{
	
}

void ATFProjectileWeapon::Attack()
{
	FHitResult Result;
	
	TraceEnemy(Result);
	
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
	FVector SocketLocation = SocketTransform.GetLocation();

	ServerAttack(Result, SocketLocation);
}

void ATFProjectileWeapon::ServerAttack_Implementation(const FHitResult& HitResult, const FVector& SocketLocation)
{
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
	MultiAttackEffects();

	SpendAmmo();
	
}


