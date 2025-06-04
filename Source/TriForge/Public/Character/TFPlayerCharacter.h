// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/TFCharacter.h"
#include "Components/TimelineComponent.h"
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

UENUM(BlueprintType)
enum class EWallRunState : uint8
{
	None,
	LeftWall,
	RightWall
};

UCLASS()
class TRIFORGE_API ATFPlayerCharacter : public ATFCharacter
{
	GENERATED_BODY()

private:
	// Movement Start --------------------------
	void GetDesiredGait();
	float CalculateMaxSpeed(float& StrafeSpeedMap);
	void UpdateMovement();
	// -------------------------- Movement End


	// Land Start ---------------------------------
	// 착지 시점에 처리해되어햘 코드
	virtual void Landed(const FHitResult& Hit) override;
	UFUNCTION()
	void OnDelayComplete();
	// --------------------------------- Land End

	
	// Wall Run Start ---------------------------
	void StartWallRun(const FVector& WallNormal);
	void StopWallRun();
	void CheckWallRun();
	// ---------------------------  Wall Run End
	
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

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bSliding;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	UCurveFloat* StrafeSpeedMapCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
	bool bJustLanded;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
	FVector LandVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* SlideMontage;

	EWallRunState WallRunState;

	FTimerHandle WallRunTimerHandle;


public:
	ATFPlayerCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PossessedBy(AController* NewController) override;
	
	// Walk < - >Sprint Start -------------
	void UpdateSprintState(bool isSprint);
	
	UFUNCTION(Server, Reliable)
	
	void ServerUpdateSprintState(bool isSprint);
	// ------------- Walk < - >Sprint Start 


	void CustomJump();


	// Slide Montage Start -------------
	void PlaySlidMontage();
	
	// Slide Request (Client -> Server)
	UFUNCTION(Server, Reliable)
	void ServerRequestSlide();

	// Play Server Slide (Server -> All Client)
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlaySlideMontage();
	//  ------------- Slide Montage End
	
	
	// Getter Start ------------
	E_Gait GetGait() const { return ECurrentGait; };
	
	bool GetJustLanded() const { return bJustLanded; };
	
	FVector GetLandVelocity() { return LandVelocity; };

	bool GetIsSliding() const { return bSliding; };

	bool GetIsSprinting() const { return bSprinting; }

	bool GetIsSWaking() const { return bWalking; }
	// ------------- Getter End
	
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

	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }

	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScope(bool bShowScope);

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

	UFUNCTION()
	void DroppedWeapon();

	// 낙사 한번만 받도록 설정한 값
	bool bIsFallingDamageApplied = false;
private:
	// 사망 시 Ragdoll 실행
	void EnableRagdoll();
};
