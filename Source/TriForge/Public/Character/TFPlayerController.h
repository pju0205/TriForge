// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/DSPlayerController.h"
#include "HUD/TFHUD.h"
#include "TFPlayerController.generated.h"

class UPlayerHealthBar;
class UMatchResultPage;
class ATFMatchPlayerState;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPawnReinitialized);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerStateReplicated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuitMenuOpen, bool, bOpen);		// Server
UCLASS()
class TRIFORGE_API ATFPlayerController : public ADSPlayerController
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputMappingContext> TFCharacterContext;

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> RotationAction;

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> SlideAction; 

	UPROPERTY(EditAnywhere, Category="Input") // Quit 버튼 설정
	TObjectPtr<UInputAction> QuitAction;

	UPROPERTY(EditAnywhere, Category="Input") 
	TObjectPtr<UInputAction> AimAction;

	UPROPERTY(EditAnywhere, Category="Input") 
	TObjectPtr<UInputAction> EquipAction;

	UPROPERTY(EditAnywhere, Category="Input") 
	TObjectPtr<UInputAction> AttackAction;
	
	FVector2d MoveDir = FVector2d::ZeroVector;
	
	void Move(const struct FInputActionValue& InputActionValue);
	void Rotation(const struct FInputActionValue& InputActionValue);
	// void CrouchStart(const struct FInputActionValue& InputActionValue);
	// void CrouchEnd(const struct FInputActionValue& InputActionValue);
	void SprintStart(const struct FInputActionValue& InputActionValue);
	void SprintEnd(const struct FInputActionValue& InputActionValue);
	void Jump(const struct FInputActionValue& InputActionValue);
	void Slide(const struct FInputActionValue& InputActionValue);

	void AimingStarted(const struct FInputActionValue& AimActionValue);
	void AimingReleased(const struct FInputActionValue& AimActionValue);
	void EquipWeapon(const struct FInputActionValue& InputActionValue);
	void WeaponAttackStarted(const struct FInputActionValue& InputActionValue);
	void WeaponAttackReleased(const struct FInputActionValue& InputActionValue);
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void GetSeamlessTravelActorList(bool bToEntry, TArray<AActor*>& ActorList) override;
	virtual void OnPossess(APawn* InPawn) override;

	// PlayerState 복제 시점 확인용
	virtual void OnRep_PlayerState() override;
	
	// ClientRestart는 서버가 클라이언트에게 호출하는 RPC이므로 Server → Client 방향
	virtual void ClientRestart_Implementation(APawn* NewPawn) override;
public:
	ATFPlayerController();
	virtual void Tick(float DeltaTime) override;

	void InitializeHealthBar(AActor* InPawn);

	bool bPawnAlive;	// 해당 Pawn 살아있는지 여부

	UFUNCTION(BlueprintCallable)
	UTFPlayerHealthComponent* GetHealthComponent() const;

	UFUNCTION()
	void SetHUDAmmo(int32 Ammo);

	UPROPERTY(BlueprintAssignable)
	FOnPlayerStateReplicated OnPlayerStateReplicated;

	UPROPERTY(BlueprintAssignable)
	FOnPawnReinitialized OnPawnReinitialized;
	
	UPROPERTY()
	ATFHUD* TFHUD;

	UPROPERTY()
	UPlayerHealthBar* HealthBar;

	UPROPERTY()
	ATFMatchPlayerState* TFMatchPS;

	UPROPERTY(BlueprintAssignable)		// Quit 버튼 설정
	FOnQuitMenuOpen OnQuitMenuOpen;
private:

	// Quit 버튼 관련
	void Input_Quit();		// Server
	bool bQuitMenuOpen;		// Server

	const FString& GetUsername() const { return Username; }
};
