

#include "Weapon/TFWeaponSpawner.h"

#include "Kismet/KismetMathLibrary.h"
#include "Weapon/TFWeapon.h"

ATFWeaponSpawner::ATFWeaponSpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SetRootComponent(StaticMesh);
	
	SpawnVolume = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnVolume"));
	SpawnVolume->SetupAttachment(RootComponent);
	
}

void ATFWeaponSpawner::BeginPlay()
{
	Super::BeginPlay();

	SpawnRandomWeapon(SpawnVolume->GetComponentLocation(), SpawnVolume->GetComponentRotation());
}

void ATFWeaponSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 매 틱마다 생성된 무기의 위치가 변경되었는지 체크
	// 위치가 변경되었다면 Delay로 설정한 초 이후에 다시 무기 생성
	if (HasAuthority())
	{
		CheckWeaponLocation();
	}
}

void ATFWeaponSpawner::CheckWeaponLocation_Implementation()
{
	if (!IsSpawnedWeaponEquipped())
	{
		StartSpawnTimer();
	}
}

bool ATFWeaponSpawner::IsSpawnedWeaponEquipped()
{
	if (!IsValid(SpawnedWeapon)) return false;
	
	if (SpawnedWeapon)
	{
		FVector SpawnedWeaponLocation = SpawnedWeapon->GetActorLocation();
		if (SpawnedWeaponLocation.Equals(SpawnVolume->GetComponentLocation()))
		{
			return true;
		}
	}
	return false;
}

void ATFWeaponSpawner::StartSpawnTimer()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(SpawnTimer)) return;
	GetWorld()->GetTimerManager().SetTimer(
			SpawnTimer,
			this,
			&ThisClass::SpawnTimerFinished,
			SpawnDelay,
			false
		);
}

void ATFWeaponSpawner::SpawnTimerFinished()
{
	SpawnRandomWeapon(SpawnVolume->GetComponentLocation(), SpawnVolume->GetComponentRotation());	
}

void ATFWeaponSpawner::SpawnRandomWeapon_Implementation(const FVector& Location, const FRotator& Rotation)
{
	if (WeaponArray.Num() > 0)
	{
		int32 RandomNum = UKismetMathLibrary::RandomIntegerInRange(0, WeaponArray.Num() - 1);
		TSubclassOf<ATFWeapon> SelectedWeapon = WeaponArray[RandomNum];

		if (SelectedWeapon)
		{
			SpawnedWeapon = GetWorld()->SpawnActor<ATFWeapon>(SelectedWeapon, Location, Rotation);
		}
	}
	// Spawn Location Debug
	/*if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan,
			FString::Printf(TEXT("%f %lf %f"),Location.X, Location.Y, Location.Z));
	}*/
	
}

