

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

	// 코드 최적화 하려고 했는데 아래의 것으로 코드를 대체하면 총알이 가끔 이상하게 날라감 Client 함수를 써야하나싶음
	/*FRotator FinalRotation;
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
	}*/
	
	MultiAttackEffects();

	SpendAmmo();
	
}


