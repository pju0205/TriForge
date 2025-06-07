
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundCue.h" 
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

	//OnComponentHit 선언부로 가서 Signature 확인하면 매개변수 알 수 있음
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	virtual void Destroyed() override;

	UPROPERTY(EditAnywhere)
	float Damage = 15.f;

	float HeadShotDamage;
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

	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticle;

	UPROPERTY(EditAnywhere)
	USoundCue* ImpactSound;
	
};
