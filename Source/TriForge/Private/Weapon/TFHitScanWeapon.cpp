// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/TFHitScanWeapon.h"

#include "Character/TFPlayerCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "TriForge/TriForge.h"

ATFHitScanWeapon::ATFHitScanWeapon()
{
	// 임시로 타입 지정하여 캐릭터 애니메이션 보기
	SetWeaponType(EWeaponType::Ewt_Rifle);
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
		FCollisionObjectQueryParams ObjectParams;
		ObjectParams.AddObjectTypesToQuery(ECC_SkeletalMesh);
		ObjectParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjectParams.AddObjectTypesToQuery(ECC_Visibility);
		
		if (World)
		{
			World->LineTraceSingleByObjectType(
				HitScanHit,
				Start,
				End,
				ObjectParams,
				FCollisionQueryParams()
			);
		}
		/*if (World)
		{
			World->LineTraceSingleByChannel(
				HitScanHit,
				Start,
				End,
				ECC_Visibility
			); 
		}*/
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
		else
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "NoDamagedChar");
			}
		}
		ImpactEffects(Hit);
	}
	BeamEffects(Hit);
	MultiAttackEffects();
	
	SpendAmmo();
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
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ImpactSound,
			Hit.ImpactPoint
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
			Hit.TraceStart,
			FRotator::ZeroRotator,
			true
		);
		if (Beam)
		{
			Beam->SetVectorParameter(FName("Target"), BeamEnd);
		}
	}
	if (MuzzleFlash)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			MuzzleFlash,
			Hit.TraceStart
		);
	}
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			FireSound,
			GetActorLocation()
		);
	}
}

FVector ATFHitScanWeapon::TraceWithScatter(const FVector& TraceStart, const FVector& HitTarget)
{
	FVector ToTargetNormailized = (HitTarget - TraceStart).GetSafeNormal();
	FVector SphereCenter = TraceStart + ToTargetNormailized * DistanceToSphere;
	FVector RandVector = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
	FVector EndLocation = SphereCenter + RandVector;
	FVector ToEndLocation = EndLocation - TraceStart;

	/*DrawDebugSphere(GetWorld(), SphereCenter, SphereRadius, 12, FColor::Red, true);
	DrawDebugSphere(GetWorld(), EndLocation, 4.f, 12, FColor::Blue, true);
	DrawDebugLine(
		GetWorld(),
		TraceStart,
		FVector(TraceStart + ToEndLocation * TRACELENGTH / ToEndLocation.Size()),
		FColor::Cyan,
		true
	);*/

	return FVector(TraceStart + ToEndLocation * TRACELENGTH / ToEndLocation.Size());
}

