// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/TFCharacter.h"
#include "TFPlayerCharacter.generated.h"

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
enum class EWallRunSide : uint8
{
	None UMETA(DisplayName = "None"),
	Left UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right")
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

	
	EWallRunSide WallRunSide = EWallRunSide::None;
	
	// 벽타기 상태 여부 (애니메이션 블루프린트에서 사용)
	UPROPERTY(BlueprintReadOnly, Category = "WallRun")
	bool bIsWallRunning = false;
	
	// 내부 사용
	FVector WallNormal;
	FVector WallRunDirection;
public:
	
	ATFPlayerCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	void UpdateSprintState(bool isSprint);
	UFUNCTION(Server, Reliable)
	void ServerUpdateSprintState(bool isSprint);
	
	void PlaySlidMontage();
	
	// 슬라이딩 요청 (클라이언트 -> 서버)
	UFUNCTION(Server, Reliable)
	void ServerRequestSlide();

	// 서버에서 슬라이딩 재생 (서버 -> 모든 클라이언트)
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlaySlideMontage();

	void StopWallRun();
	void WallRunJump();
	void StartWallRun(EWallRunSide Side);
	bool TraceWall(const FVector& Start, const FVector& Direction, float Distance, FVector& OutHitNormal);
	void CheckForWallRun();
	
	UFUNCTION(BlueprintPure)
	EWallRunSide GetWallRunSide() const { return WallRunSide; }
	
	UFUNCTION(BlueprintPure)
	bool GetIsWallRunning() const { return bIsWallRunning; }

	E_Gait GetGait() const {return ECurrentGait;};
	bool GetIsSliding() const {return bSliding;};
	bool GetJustLanded() const {return bJustLanded;};
	FVector GetLandVelocity() {return LandVelocity;};

	
};

