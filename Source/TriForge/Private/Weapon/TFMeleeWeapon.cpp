// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/TFMeleeWeapon.h"

#include "Character/TFPlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "TriForge/TriForge.h"

ATFMeleeWeapon::ATFMeleeWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	
	SetWeaponType(EWeaponType::Ewt_Knife);
	SetWeaponClass(EWeaponClass::Ewc_MeleeWeapon);

	TraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("CollisionBoxStart"));
	TraceStart->SetupAttachment(RootComponent);
	
	TraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("CollisionBoxEnd"));
	TraceEnd->SetupAttachment(RootComponent);
	
}

void ATFMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();

	HeadShotDamage = Damage * 1.5f;
}

void ATFMeleeWeapon::Attack()
{
	Super::Attack();
	
	ServerAttackEffects();

	// 근접무기 애니메이션 ~ 대미지 까지 흐름도
	// 좌클릭 -> ... -> WeaponComponent::Attacking -> MeleeWeapon::Attack() -> ServerAttackEffects() -> MultiAttackEffects()
	// -> Montage내의 ANS -> StartTraceTimer() -> TraceEnemy() -> ServerAttack() -> StopTraceTimer()
}

// ANS의 Begin ~ End동안 0.02초마다 SphereTrace
void ATFMeleeWeapon::StartTraceTimer()
{
	AlreadyHitActors.Empty();

	GetWorld()->GetTimerManager().SetTimer(
		TraceTimerHandle,
		this,
		&ATFMeleeWeapon::TraceEnemy,
		0.02f,
		true
	);
}

void ATFMeleeWeapon::StopTraceTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(TraceTimerHandle);
	AlreadyHitActors.Empty();
}

void ATFMeleeWeapon::TraceEnemy()
{
	FVector CurrentTraceStart = TraceStart->GetComponentLocation();
	FVector CurrentTraceEnd = TraceEnd->GetComponentLocation();

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	FHitResult HitResult;
	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		CurrentTraceStart,
		CurrentTraceEnd,
		FQuat::Identity,
		ECC_SkeletalMesh,
		FCollisionShape::MakeCapsule(10.f,10.f),
		Params
	);

	// Timer의 시작 ~ 중지 시간 동안 같은 Actor에게 중복해서 Damage 감소 불가
	if (bHit && !AlreadyHitActors.Contains(HitResult.GetActor()))
	{
		AlreadyHitActors.Add(HitResult.GetActor());
		ServerAttack(HitResult);
	}
	
}

void ATFMeleeWeapon::ServerAttackEffects_Implementation()
{
	MultiAttackEffects();
}

void ATFMeleeWeapon::MultiAttackEffects_Implementation()
{
	PlayAttackMontage();
}

void ATFMeleeWeapon::ServerAttack_Implementation(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit)
	{
		AActor* DamagedActor = HitResult.GetActor();

		if (DamagedActor)
		{
			ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
			if (OwnerCharacter && DamagedActor != OwnerCharacter)
			{
				AController* OwnerController = OwnerCharacter->Controller;
				if (OwnerController)
				{
					float CalculatedDamage = HitResult.BoneName.ToString() == FString("head") ? HeadShotDamage : Damage;
					UGameplayStatics::ApplyDamage(DamagedActor, CalculatedDamage, OwnerController, this, UDamageType::StaticClass());
				}
			}
		}
	}
	
}

void ATFMeleeWeapon::BeginTrace()
{
	StartTraceTimer();
}

void ATFMeleeWeapon::EndTrace()
{
	StopTraceTimer();
}





