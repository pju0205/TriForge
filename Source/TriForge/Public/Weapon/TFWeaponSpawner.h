
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TFWeaponSpawner.generated.h"

class ATFWeapon;

UCLASS()
class TRIFORGE_API ATFWeaponSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ATFWeaponSpawner();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere)
	USceneComponent* SpawnVolume;
	
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<ATFWeapon>> WeaponArray;

	UFUNCTION(Server, Reliable)
	void SpawnRandomWeapon(const FVector& Location, const FRotator& Rotation);

	UPROPERTY(VisibleAnywhere)
	ATFWeapon* SpawnedWeapon;

	UFUNCTION(Server, Reliable)
	void CheckWeaponLocation();
	
	bool IsSpawnedWeaponEquipped();

	FTimerHandle SpawnTimer;

	void StartSpawnTimer();

	void SpawnTimerFinished();

	UPROPERTY(EditAnywhere)
	float SpawnDelay = 3.f;
};
