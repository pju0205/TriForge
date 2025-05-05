// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/TFMeleeWeapon.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "TriForge/TriForge.h"

ATFMeleeWeapon::ATFMeleeWeapon()
{
	SetWeaponType(EWeaponType::Ewt_Knife);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(RootComponent);

	CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);

	CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECR_Block);
}

void ATFMeleeWeapon::Attack(const FHitResult& HitResult, const FVector& SocketLocation)
{
	Super::Attack(HitResult, SocketLocation);

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

void ATFMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();


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
