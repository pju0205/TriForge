
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TFProjectile.generated.h"

class UProjectileMovementComponent;
class UBoxComponent;

UCLASS()
class TRIFORGE_API ATFProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ATFProjectile();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere)
	UBoxComponent* CollisionBox;

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere)
	UParticleSystem* Tracer;

	UPROPERTY()
	UParticleSystemComponent* TracerComponent;
};
