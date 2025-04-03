
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TFWeapon.generated.h"

class USphereComponent;

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	Ews_Initial UMETA(DisplayName = "Inital State"),
	Ews_Equipped UMETA(DisplayName = "Equipped"),
	Ews_Dropped UMETA(DisplayName = "Dropped"),

	Ews_Max UMETA(DisplayName = "DefaultMax")
};

UCLASS()
class TRIFORGE_API ATFWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	ATFWeapon();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void SphereBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult & SweepResult);

	UFUNCTION()
	void SphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

public:	
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE void SetWeaponState(EWeaponState State) {WeaponState = State;}

private:
	UPROPERTY(VisibleAnywhere, Category = "WeaponProperties")
	USkeletalMeshComponent* WeaponMesh;
	
	UPROPERTY(VisibleAnywhere, Category = "WeaponProperties")
	USphereComponent* WeaponSphere;

	UPROPERTY(VisibleAnywhere)
	EWeaponState WeaponState;
};
