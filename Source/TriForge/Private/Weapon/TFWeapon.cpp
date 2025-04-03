

#include "Weapon/TFWeapon.h"

#include "TFMyCharacter.h"
#include "Components/SphereComponent.h"

ATFWeapon::ATFWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	WeaponMesh->SetCollisionResponseToAllChannels(ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 무기와 캐릭터가 일정거리로 좁혀졌는지 확인하기 위한 Sphere
	WeaponSphere = CreateDefaultSubobject<USphereComponent>(TEXT("WeaponSphere"));
	WeaponSphere->SetupAttachment(RootComponent);

	WeaponSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	WeaponSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
}

void ATFWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		WeaponSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

		WeaponSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::ATFWeapon::SphereBeginOverlap);
		WeaponSphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::ATFWeapon::SphereEndOverlap);
	}
}

void ATFWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATFWeapon::SphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ATFMyCharacter* TFCharacter = Cast<ATFMyCharacter>(OtherActor);
	if (TFCharacter)
	{
		// WeaponSphere에 캐릭터가 Overlap이면 OveralppingWeapon 값을 Overlap되어있는 무기로 하여
		// 현재 WeaponSphere에 캐릭터가 Overlap 되어있음을 알 수 있음
		TFCharacter->SetOverlappingWeapon(this);
		
	}
}

void ATFWeapon::SphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ATFMyCharacter* TFCharacter = Cast<ATFMyCharacter>(OtherActor);
	if (TFCharacter)
	{
		// Sphere 범위 밖으로 나가면 Overlapping Weapon = nullptr로 해줌으로써
		// Overlap 되어있는 Weapon이 없다는 것을 의미
		TFCharacter->SetOverlappingWeapon(nullptr);
	}
}

