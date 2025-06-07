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
class USoundWave;
// class UTFAnimInstance;

UENUM(BlueprintType)
enum class E_Gait : uint8
{
	Walk	UMETA(DisplayName = "Walk"),
	Sprint	UMETA(DisplayName = "Sprint")
};

UENUM(BlueprintType)
enum class E_WallRunState : uint8
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
	
	// Movement Function Start ----------------------------------------------------
	void GetDesiredGait();
	float CalculateMaxSpeed(float& StrafeSpeedMap);
	void UpdateMovement();
	// ---------------------------------------------------- Movement Function End


	// Land Function Start ----------------------------------------------------------------------
	virtual void Landed(const FHitResult& Hit) override; 	// 착지 시점에 처리해되어햘 코드
	
	UFUNCTION()
	void OnDelayComplete();
	// ---------------------------------------------------------------------- Land Function End

	
	// Wall Run Function Start ---------------------------------------------------------
	void StartWallRun(const FVector& WallNormal, E_WallRunState NewState);
	void StopWallRun();
	void CheckWallRun();
	// ---------------------------------------------------------  Wall Run Function End

	
	UFUNCTION(Server, Reliable)
	void ServerCustomJump(); // 점프 상태 서버에 전달
	
	
	// Slide Function Start -------------------------------------------------
	UFUNCTION(Server, Reliable)
	void ServerRequestSlide(); // Slide Request (Client -> Server)
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlaySlideMontage(); // Play Server Slide (Server -> All Client)
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastStopSlideEffects();
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartSlideSound();
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastStopSlideSound();

	FTimerHandle SlideEndTimerHandle;	// 서버에서 튕김 지역변수 타이머 만들면
	void OnSlideMontageEnded();
	// ------------------------------------------------- Slide Function End
	
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowedClasses = "true"))
	TObjectPtr<UCameraComponent> Camera = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowedClasses = "true"))
	TObjectPtr<UCameraComponent> Camera3p = nullptr;

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

	
	// Slide Value Start --------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* SlideMontage;
	
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* SlideSoundWave;

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundAttenuation* SlideAttenuationSettings; // 핵심

	UPROPERTY()
	UAudioComponent* SlideAudioComponent;
	//  -------------------------------------------------------------------------- Slide Value End


	// Wall Run Value Start --------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
	E_WallRunState WallRunState;

	FTimerHandle WallRunTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bWallRun;
	// --------------------------------------------------------------------------Wall Run Value End

	
public:
	ATFPlayerCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PossessedBy(AController* NewController) override;

	
	// Walk And Sprint Function Start ----------------------------------------------------------------------
	void UpdateSprintState(bool isSprint);
	
	UFUNCTION(Server, Reliable)
	
	void ServerUpdateSprintState(bool isSprint);
	// ---------------------------------------------------------------------- Walk And Sprint Function End


	// Controller Callable Start ---------------------------------------------------------------------------
	// void UpdateSprintState(bool isSprint); // SprintStart & SprintEnd
	
	void CustomJump(); // Jump
	
	void PlaySlidMontage(); // Slide
	// --------------------------------------------------------------------------- Controller Callable End
	
	
	
	// Getter Start --------------------------------------------------------------------
	E_Gait GetGait() const { return ECurrentGait; }
	
	bool GetJustLanded() const { return bJustLanded; }
	
	FVector GetLandVelocity() { return LandVelocity; }

	bool GetIsSliding() const { return bSliding; }

	bool GetIsSprinting() const { return bSprinting; }

	bool GetIsSWaking() const { return bWalking; }

	bool GetIsWallRun() const {return bWallRun; }

	E_WallRunState GetWallRunState() const { return WallRunState; }
	// -------------------------------------------------------------------- Getter End

	
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

	bool bIsAttacking;

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

	/*UFUNCTION()
	void DroppedWeapon();*/

	// 낙사 한번만 받도록 설정한 값
	bool bIsFallingDamageApplied = false;

	// 죽었을 때 카메라 관련
	void SwitchToDeathCamera();
	void ResetToFirstPersonCamera();
private:
	// 사망 시 Ragdoll 실행
	void EnableRagdoll();

	// 맞을 때 소리 배열
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TArray<USoundBase*> HitReactionSounds;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundAttenuation* SoundAttenuation;
	
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayHitSound(FVector Location);
	
	UFUNCTION(Client, Reliable)
	void ClientPlayDamageEffect();

	void PlayDamageEffect();
};
