
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundCue.h"
#include "TFWeapon.generated.h"

class UWidgetComponent;
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

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Ewt_Rifle UMETA(DisplayName = "Rifle"),
	Ewt_ShotGun UMETA(DisplayName = "ShotGun"),
	Ewt_Pistol UMETA(DisplayName = "Pistol"),
	Ewt_SniperRifle UMETA(DisplayName = "Sniper"),
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

	UPROPERTY(EditAnywhere, Category = "Settings")
	UTexture2D* WeaponIcon;

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
	
	UFUNCTION()
	virtual void OnRep_WeaponState();
	
public:	
	virtual void Tick(float DeltaTime) override;
	
	void SetWeaponState(EWeaponState State);
	FORCEINLINE EWeaponState GetWeaponState() const { return WeaponState; }
	FORCEINLINE void SetWeaponClass(EWeaponClass Class) { WeaponClass = Class; }
	FORCEINLINE void SetWeaponType(EWeaponType Type) { WeaponType = Type; }
	FORCEINLINE EWeaponClass GetWeaponClass() const {return WeaponClass;}
	FORCEINLINE EWeaponType GetWeaponType() const {return WeaponType;}
	
	FORCEINLINE USphereComponent* GetWeaponSphere() const {return WeaponSphere;}

	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Attack();
	
	// 무기에 맞는 캐릭터 애니메이션 
	UPROPERTY(EditAnywhere)
	UAnimMontage* AttackMontage;
	
	void PlayAttackMontage();

	virtual void Dropped();
	
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() {return WeaponMesh;}
	
	UPROPERTY(EditAnywhere, Category = "Settings")
	float AttackDelay = .15;;
	
	UPROPERTY(EditAnywhere, Category = "Settings")
	bool bAutomatic = false;;
	
	// 무기 자체의 애니메이션
	UPROPERTY(EditAnywhere, Category = "Settings")
	UAnimationAsset* RangedWeaponAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FVector RightHandOffsetLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FRotator RightHandOffsetRotation;

	UPROPERTY(EditAnywhere, Category = "Settings")
	USoundCue* EquipSound;

	UPROPERTY(EditAnywhere, Category = "Settings")
	USoundCue* DropSound;

	UFUNCTION()
	void PlayEquipSound();
	
	UFUNCTION()
	void PlayDropSound();
	
	void ShowPickupWidget(bool bShow);

	void ShowWeaponIcon();
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

	UPROPERTY(EditAnywhere, Category = "Settings")
	float ZoomedFOV = 30.f;

	UPROPERTY(EditAnywhere, Category = "Settings")
	float ZoomInterpSpeed = 20.f;

	 /* PickupWidget Settings in Blueprint
	 Category = "User Interface"
	 Space : Screen, WidgetClass : WBP_PickupWidget
	 Draw at Desired Size = true */
	UPROPERTY(VisibleAnywhere, Category = "Settings")
	UWidgetComponent* PickupWidget;
};
