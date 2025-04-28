
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

UENUM()
enum class EWeaponClass : uint8
{
	Ewc_RangedWeapon UMETA(DisplayNmae = "RangedWeapon"),
	Ewc_MeleeWeapon UMETA(DisplayName = "MeleeWeapon"),

	Ewt_Max UMETA(DisplayName = "DefaultMax")
};

UENUM()
enum class EWeaponType : uint8
{
	Ewt_Rifle UMETA(DisplayName = "Rifle"),
	Ewt_ShotGun UMETA(DisplayName = "ShotGun"),
	Ewt_Knife UMETA(DisplayName = "Knife"),
	Ewt_Hammer UMETA(DisplayName = "Hammer"),

	Ewt_Max UMETA(DisplayName = "DefaultMax")
};

UCLASS()
class TRIFORGE_API ATFWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	ATFWeapon();

	//Texture for Crosshair
	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	UTexture2D* CrosshairsCenter;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	UTexture2D* CrosshairsLeft;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	UTexture2D* CrosshairsRight;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	UTexture2D* CrosshairsTop;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	UTexture2D* CrosshairsBottom;

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


	// 무기에 맞는 캐릭터 애니메이션 
	UPROPERTY(EditAnywhere)
	UAnimMontage* AttackMontage;

	
public:	
	virtual void Tick(float DeltaTime) override;

	
	void SetWeaponState(EWeaponState State);
	FORCEINLINE void SetWeaponClass(EWeaponClass Class) { WeaponClass = Class; }
	FORCEINLINE void SetWeaponType(EWeaponType Type) { WeaponType = Type; }
	FORCEINLINE EWeaponClass GetWeaponClass() const {return WeaponClass;}
	FORCEINLINE EWeaponType GetWeaponType() const {return WeaponType;}
	
	FORCEINLINE USphereComponent* GetWeaponSphere() const {return WeaponSphere;}

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Attack(const FVector& HitTarget);
	void PlayAttackMontage();

	virtual void Dropped();
	
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() {return WeaponMesh;}
	
	UPROPERTY(EditAnywhere)
	float AttackDelay = .15;;
	
	UPROPERTY(EditAnywhere)
	bool bAutomatic = false;;

	
private:
	UPROPERTY(VisibleAnywhere, Category = "WeaponProperties")
	USkeletalMeshComponent* WeaponMesh;
	
	UPROPERTY(VisibleAnywhere, Category = "WeaponProperties")
	USphereComponent* WeaponSphere;

	UPROPERTY(ReplicatedUsing= OnRep_WeaponState, VisibleAnywhere)
	EWeaponState WeaponState;

	UPROPERTY(VisibleAnywhere)
	EWeaponClass WeaponClass;

	UPROPERTY(VisibleAnywhere)
	EWeaponType WeaponType;
	
	UFUNCTION()
	void OnRep_WeaponState();
	
	// 무기 자체의 애니메이션
	UPROPERTY(EditAnywhere)
	UAnimationAsset* RangedWeaponAnimation;
	
};
