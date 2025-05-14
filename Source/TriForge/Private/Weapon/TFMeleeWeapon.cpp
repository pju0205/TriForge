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

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(RootComponent);

	
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECR_Overlap);
	/*CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);*/
	

	CollisionBoxStart = CreateDefaultSubobject<USceneComponent>(TEXT("CollisionBoxStart"));
	CollisionBoxStart->SetupAttachment(RootComponent);
	
	CollisionBoxEnd = CreateDefaultSubobject<USceneComponent>(TEXT("CollisionBoxEnd"));
	CollisionBoxEnd->SetupAttachment(RootComponent);
}

void ATFMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBoxBeginOverlap);

}

void ATFMeleeWeapon::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ATFPlayerCharacter* OverlappedCharacter = Cast<ATFPlayerCharacter>(OtherActor);
	if (OverlappedCharacter && OverlappedCharacter == GetOwner()) return;
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red, "CollisionBoxOverlap");
	}
	FHitResult BoxHit;
	UKismetSystemLibrary::BoxTraceSingle(this,
		CollisionBoxStart->GetComponentLocation(),
		CollisionBoxEnd->GetComponentLocation(),
		FVector(2.f,2.f,2.f),
		CollisionBoxStart->GetComponentRotation(),
		TraceTypeQuery1,
		false,
		{},
		EDrawDebugTrace::ForDuration,
		BoxHit,
		true
		);
	
	ServerAttack(BoxHit);
}

void ATFMeleeWeapon::Attack()
{
	Super::Attack();
	
	 ServerAttackEffects();
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
					UGameplayStatics::ApplyDamage(DamagedActor, Damage, OwnerController, this, UDamageType::StaticClass());
				}
			}
		}
	}
}

void ATFMeleeWeapon::OnRep_WeaponState()
{
	Super::OnRep_WeaponState();

	/*EWeaponState CurrentState = GetWeaponState();
	switch (CurrentState)
	{
	case EWeaponState::Ews_Equipped:
		SetBoxCollision(true);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Cyan, "Equipped");
		}
		/*CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECR_Overlap);#1#
		break;
	case EWeaponState::Ews_Dropped:
		SetBoxCollision(false);
		/*CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);#1#
		break;
	}*/
}


