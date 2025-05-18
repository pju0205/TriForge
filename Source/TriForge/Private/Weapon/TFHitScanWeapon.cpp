// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/TFHitScanWeapon.h"

#include "Character/TFPlayerCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

ATFHitScanWeapon::ATFHitScanWeapon()
{
	// 임시로 타입 지정하여 캐릭터 애니메이션 보기
	SetWeaponType(EWeaponType::EWt_Pistol);
}

void ATFHitScanWeapon::Attack()
{
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if (MuzzleFlashSocket)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FHitResult HitResult;

		TraceEnemy(HitResult);
		FVector Start = SocketTransform.GetLocation();
		FVector End;
		
		if (HitResult.bBlockingHit)
		{
			End = Start + (HitResult.ImpactPoint - Start) * 1.25;
		}
		else
		{
			End = Start + (HitResult.TraceEnd - Start) * 1.25;
		}
		
		FHitResult HitScanHit;
		UWorld* World = GetWorld();
		if (World)
		{
			World->LineTraceSingleByChannel(
				HitScanHit,
				Start,
				End,
				ECC_Visibility
			); 
		}
		ServerAttack(HitScanHit);
	}
	
}

void ATFHitScanWeapon::ServerAttack_Implementation(const FHitResult& Hit)
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();
	
	if (Hit.bBlockingHit && InstigatorController)
	{
		ATFPlayerCharacter* DamagedCharacter = Cast<ATFPlayerCharacter>(Hit.GetActor());
		if (DamagedCharacter)
		{
			UGameplayStatics::ApplyDamage(
				DamagedCharacter,
				Damage,
				InstigatorController,
				this,
				UDamageType::StaticClass()
			);
		}
		ImpactEffects(Hit);
	}
	BeamEffects(Hit);
	AttackEffects();
	SpendAmmo();
}

void ATFHitScanWeapon::AttackEffects_Implementation()
{
	PlayAttackMontage();
	GetWeaponMesh()->PlayAnimation(RangedWeaponAnimation, false);
}


void ATFHitScanWeapon::ImpactEffects_Implementation(const FHitResult& Hit)
{
	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ImpactParticles,
			Hit.ImpactPoint,
			Hit.ImpactNormal.Rotation()
		);
	}
}


void ATFHitScanWeapon::BeamEffects_Implementation(const FHitResult& Hit)
{
	FVector BeamEnd;
	if (BeamParticles)
	{
		if (Hit.bBlockingHit)
		{
			BeamEnd = Hit.ImpactPoint;
		}
		else
		{
			BeamEnd = Hit.TraceEnd;
		}
		UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			BeamParticles,
			Hit.TraceStart
		);
		if (Beam)
		{
			Beam->SetVectorParameter(FName("Target"), BeamEnd);
		}
	}
}
