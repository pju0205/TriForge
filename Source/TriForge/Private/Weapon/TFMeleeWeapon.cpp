// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/TFMeleeWeapon.h"

#include "Character/TFWeaponCharacter.h"
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
	ATFWeaponCharacter* OverlappedCharacter = Cast<ATFWeaponCharacter>(OtherActor);
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
	Attack(BoxHit,FVector(0,0,0));
}

void ATFMeleeWeapon::Attack_Implementation(const FHitResult& HitResult, const FVector& SocketLocation)
{
	Super::Attack(HitResult, SocketLocation);
	
	//AttackEffects();
	if (HitResult.bBlockingHit)
	{
		
		AActor* DamagedActor = HitResult.GetActor();

		if (DamagedActor)
		{
			ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
			if (OwnerCharacter)
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




void ATFMeleeWeapon::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                           FVector NormalImpulse, const FHitResult& Hit)
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter)
	{
		AController* OwnerController = OwnerCharacter->Controller;
		if (OwnerController)
		{
			UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, UDamageType::StaticClass());
			
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

void ATFMeleeWeapon::SetBoxCollision_Implementation(bool bCollisionOn)
{
	/*if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Cyan, "setboxCollision");
	}
	if (bCollisionOn)
	{
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECR_Overlap);
	}
	else
	{
		CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}*/
}

void ATFMeleeWeapon::AttackEffects_Implementation()
{
	PlayAttackMontage();
}
