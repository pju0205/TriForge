

#include "Weapon/TFProjectile.h"

#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "TriForge/TriForge.h"

ATFProjectile::ATFProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	CollisionBox= CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);

	// World 안에서 동적으로 움직이는 오브젝트
	CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
	
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);

	CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECR_Block);
	
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;

	// initial speed, max speed 블루프린트에서 설정 필요 or 코드로 작성 코드로 작성시 define으로 해도 될듯
	ProjectileMovementComponent->InitialSpeed = 80000;
	ProjectileMovementComponent->MaxSpeed = 80000;

	InitialLifeSpan = 3.f;
}

void ATFProjectile::BeginPlay()
{
	Super::BeginPlay();

	HeadShotDamage = Damage * 1.5;
	
	if (Tracer)
	{
		TracerComponent = UGameplayStatics::SpawnEmitterAttached(
			Tracer,
			CollisionBox,
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition
		);
	}
	
	if (HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
	}
}


void ATFProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATFProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	
	Destroy();
}

void ATFProjectile::Destroyed()
{
	Super::Destroyed();

	// 소리와 이펙트에 대해서 RPC 처리를 하지 않아도 Destroyed 함수를 이용하면 클라이언트에도 동일한 효과를 제공 가능
	if (ImpactParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle,GetActorTransform());
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
}
