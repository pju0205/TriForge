

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
	
	DrawDebugLine(GetWorld(), SocketLocation, HitResult.ImpactPoint, FColor::Red, false, 3.f);
	DrawDebugLine(GetWorld(), SocketLocation, HitResult.TraceEnd, FColor::Yellow, false, 3.f);*/
	

	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	FRotator FinalRotation;
	if (HitResult.bBlockingHit) FinalRotation = (HitResult.ImpactPoint - SocketLocation).Rotation();
	else FinalRotation = (HitResult.TraceEnd - SocketLocation).Rotation();
	
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
				FinalRotation,
				SpawnParams
			);
		}
	}
	
	MultiAttackEffects();

	SpendAmmo();
	
}


