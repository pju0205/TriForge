
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

		// 대미지 계산을 위해 TMap<ATFPlayerCharacter*, uint32> HitMap을 이용
		// 탄 하나당 ApplyDamage가 아닌 맞은 탄의 개수를 세어 Damage * Count로 Total Damage 계산
		HitMap.Empty();
		HeadShotHitMap.Empty();
		
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

void ATFShotgun::ServerShotgunAttack_Implementation(ATFPlayerCharacter* DamagedCharacter, float TotalDamage)
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();
	if (InstigatorController)
	{
		UGameplayStatics::ApplyDamage(
			DamagedCharacter,
			TotalDamage,
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
			const bool bHeadShot = ShotgunHit.BoneName.ToString() == FString("head");
			// HeadShot이면 HeadShotHitMap에 아니면 HitMap에 저장
			if (bHeadShot)
			{
				if (HeadShotHitMap.Contains(TFPlayerCharacter)) HeadShotHitMap[TFPlayerCharacter]++;
				else HeadShotHitMap.Emplace(TFPlayerCharacter, 1);
			}
			else
			{
				if (HitMap.Contains(TFPlayerCharacter)) HitMap[TFPlayerCharacter]++;
				else HitMap.Emplace(TFPlayerCharacter, 1);
			}
			
		}
	}

	// 기존에는 어떤 캐릭터가 몇번 대미지를 받아야하는지 ServerShotgunAttack()으로 보냈다면
	// 변경 후에는 어떤 캐릭터가 대미지를 총합 얼마를 줘야 하는지 미리 계산해서 보낸다.
	TMap<ATFPlayerCharacter*, float> DamageMap;
	for (auto Pair : HitMap)
	{
		if (Pair.Key)
		{
			DamageMap.Emplace(Pair.Key, Pair.Value * Damage);
		}
	}
	
	for (auto HeadShotPair : HeadShotHitMap)
	{
		if (HeadShotPair.Key)
		{
			// DamageMap에 캐릭터가 중복이면 해당 인덱스의 값 += 헤드샷 대미지 * 맞은 횟수 
			if (DamageMap.Contains(HeadShotPair.Key)) DamageMap[HeadShotPair.Key] += HeadShotPair.Value* HeadShotDamage;
			else DamageMap.Emplace(HeadShotPair.Key, HeadShotPair.Value * HeadShotDamage);
			
		}
	}
	for (auto DamagePair : DamageMap)
	{
		if (DamagePair.Key)
		{
			ServerShotgunAttack(DamagePair.Key, DamagePair.Value);
		}
	}
	
	HitMap.Empty();
	HeadShotHitMap.Empty();
}
