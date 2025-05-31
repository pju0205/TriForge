// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/TFCharacter.h"
#include "TFPlayerCharacter.generated.h"


class UTFPlayerHealthComponent;
class UTFEliminationComponent;
class ATFPlayerState;
class ATFPlayerController;
class UTFWeaponComponent;
class ATFWeapon;
class USpringArmComponent;
class UCameraComponent;
class UCurveFloat;
class UAnimMontage;
// class UTFAnimInstance;

UENUM(BlueprintType)
enum class E_Gait : uint8
{
	Walk	UMETA(DisplayName = "Walk"),
	Sprint	UMETA(DisplayName = "Sprint")
};

UCLASS()
class TRIFORGE_API ATFPlayerCharacter : public ATFCharacter
{
	GENERATED_BODY()

private:
	void GetDesiredGait();
	float CalculateMaxSpeed(float& StrafeSpeedMap);
	void UpdateMovement();
	
	virtual void Landed(const FHitResult& Hit) override;
	UFUNCTION()
	void OnDelayComplete();

	void SetSlideMontage(bool bisSlideDir);
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowedClasses = "true"))
	TObjectPtr<UCameraComponent> Camera = nullptr;

	// UPROPERTY(BlueprintReadOnly, Category = "Animation")
	// UTFAnimInstance* TFAnimInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	E_Gait ECurrentGait;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FVector WalkSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FVector SprintSpeed;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bSprinting;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bWalking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	UCurveFloat* StrafeSpeedMapCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
	bool bJustLanded;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
	FVector LandVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* SlideMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* ForwardSlide_Montage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* BackSlide_Montage; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* RightSlide_Montage; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* LeftSlide_Montage; 
	
public:
	ATFPlayerCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	void UpdateSprintState(bool isSprint);
	UFUNCTION(Server, Reliable)
	void ServerUpdateSprintState(bool isSprint);
	
	void SetSlideDir(float Forward, float Right); 
	void isPlayingSlideMontage(float Forward, float Right); 
	void PlaySlidMontage();
	
	E_Gait GetGait() const {return ECurrentGait;};
	bool GetJustLanded() const {return bJustLanded;};
	FVector GetLandVelocity() {return LandVelocity;};

	void CleanupBeforeMapTravel();

//Weapon
private:
	// 데미지 함수
	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:
	virtual void PostInitializeComponents() override;

	void SetOverlappingWeapon(ATFWeapon* Weapon);

	bool IsWeaponEquipped();
	bool IsAiming();

	void AimButtonPressed();
	void AimButtonReleased();
	void EquipButtonPressed();
	void AttackButtonPressed();
	void AttackButtonReleased();

	UTFWeaponComponent* GetWeaponComponent();

	ATFWeapon* GetEquippedWeapon();

private:
	UPROPERTY(VisibleAnywhere)
	UTFWeaponComponent* WeaponComponent;

	UPROPERTY(ReplicatedUsing=OnRep_OverlappingWeapon)
	ATFWeapon* OverlappingWeapon;

	UPROPERTY()
	ATFPlayerController* TFPlayerController;

	UPROPERTY()
	ATFPlayerState* TFPlayerState;
	
	
	UFUNCTION()
	void OnRep_OverlappingWeapon(ATFWeapon* LastWeapon);

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastEquipButtonPressed();

// Health 관련
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTFPlayerHealthComponent> HealthComponent;

	// 사망 처리 함수 - HealthComponent에서 호출
	UFUNCTION()
	void OnDeathStarted(AActor* DyingActor, AActor* DeathInstigator);

protected:
	// 사망 애니메이션 재생
	void PlayDirectionalDeathMontage(AActor* DeathInstigator);

	// 방향에 따른 애니메이션 선택
	UAnimMontage* GetDirectionalDeathMontage(const FVector& HitDirection) const;

	// 사망 애니메이션들
	UPROPERTY(EditDefaultsOnly, Category = "Death")
	UAnimMontage* DeathMontage_Front;

	UPROPERTY(EditDefaultsOnly, Category = "Death")
	UAnimMontage* DeathMontage_Back;

	UPROPERTY(EditDefaultsOnly, Category = "Death")
	UAnimMontage* DeathMontage_Left;

	UPROPERTY(EditDefaultsOnly, Category = "Death")
	UAnimMontage* DeathMontage_Right;

	UPROPERTY(EditDefaultsOnly, Category = "Death")
	UAnimMontage* DeathMontage_FrontLeft;

	UPROPERTY(EditDefaultsOnly, Category = "Death")
	UAnimMontage* DeathMontage_FrontRight;
};


