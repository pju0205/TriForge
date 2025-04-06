

#include "Weapon/TFProjectile.h"

#include "Components/BoxComponent.h"

ATFProjectile::ATFProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionBox= CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);

	// World 안에서 동적으로 움직이는 오브젝트
	CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
	
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);

	CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
}

void ATFProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATFProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

