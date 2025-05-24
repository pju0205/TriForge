
#include "Weapon/TFShotgun.h"

#include "Character/TFPlayerCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

ATFShotgun::ATFShotgun()
{
	SetWeaponType(EWeaponType::Ewt_ShotGun);
}

void ATFShotgun::Attack()
{
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if (MuzzleFlashSocket)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FHitResult HitResult;
		
		TraceEnemy(HitResult);
		FVector Start = SocketTransform.GetLocation();
		FVector End;

		HitMap.Empty();
		
		if (HitResult.bBlockingHit)
		{
			End = HitResult.ImpactPoint;
		}
		else
		{
			End = HitResult.TraceEnd;
		}
		
		CountingHit(Start, End);
	}
	
	ServerAttackEffects();
}

void ATFShotgun::ServerShotgunAttack_Implementation(ATFPlayerCharacter* DamagedCharacter, uint32 Times)
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();
	if (InstigatorController)
	{
		UGameplayStatics::ApplyDamage(
			DamagedCharacter,
			Damage * Times,
			InstigatorController,
			this,
			UDamageType::StaticClass()
		);
	}
	
}

void ATFShotgun::ServerAttackEffects_Implementation()
{
	MultiAttackEffects();
	SpendAmmo();
	
}

void ATFShotgun::ServerBeamEffect_Implementation(const FHitResult& Hit)
{
	BeamEffects(Hit);
}


void ATFShotgun::ServerImpactEffect_Implementation(const FHitResult& Hit)
{
	ImpactEffects(Hit);
}

void ATFShotgun::CountingHit(const FVector& Start, const FVector& End)
{
	for (uint32 i = 0; i < NumberOfFrangible; i++)
	{
		FVector EndLocation = TraceWithScatter(Start, End);
		UWorld* World = GetWorld();
		FHitResult ShotgunHit;
				
		if (World)
		{
			World->LineTraceSingleByChannel(
				ShotgunHit,
				Start,
				EndLocation,
				ECC_Visibility
			);
		}

		ServerBeamEffect(ShotgunHit);
		if (ShotgunHit.bBlockingHit)
		{
			ServerImpactEffect(ShotgunHit);
		}
		
		ATFPlayerCharacter* TFPlayerCharacter = Cast<ATFPlayerCharacter>(ShotgunHit.GetActor());
		if (TFPlayerCharacter)
		{
			if (HitMap.Contains(TFPlayerCharacter))
			{
				HitMap[TFPlayerCharacter]++;
			}
			else
			{
				HitMap.Emplace(TFPlayerCharacter, 1);
			}
		}
	}
	
	for (auto Pair : HitMap)
	{
		if (Pair.Key)
		{
			ServerShotgunAttack(Pair.Key, Pair.Value);
			
		}
	}
	
	HitMap.Empty();
}
