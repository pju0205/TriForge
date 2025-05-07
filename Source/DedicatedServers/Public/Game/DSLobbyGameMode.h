// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DSGameModeBase.h"
#include "GameLiftServerSDK.h"
#include "DSLobbyGamemode.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(LogDSGameMode, Log, All);
class UDSGameInstanceSubsystem;
/**
 *
 */
UCLASS()
class DEDICATEDSERVERS_API ADSLobbyGameMode : public ADSGameModeBase
{
	GENERATED_BODY()

public:
	ADSLobbyGameMode();
	virtual void PostLogin(APlayerController* NewPlayer) override;

protected:
	virtual void BeginPlay() override;
    
	virtual void OnCountdownTimerFinished(ECountdownTimerType Type) override;
	void CancelCountdown();
	virtual void Logout(AController* Exiting) override;
	virtual void InitSeamlessTravelPlayer(AController* NewController) override;
 
	UPROPERTY()
	ELobbyStatus LobbyStatus;
 
	UPROPERTY(EditDefaultsOnly)
	int32 MinPlayers;
 
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> MapToTravelTo;

private:

	UPROPERTY()
	TObjectPtr<UDSGameInstanceSubsystem> DSGameInstanceSubsystem;

	UPROPERTY(EditDefaultsOnly)
	FCountdownTimerHandle LobbyCountdownTimer;

	void InitGameLift();
	void SetServerParameters(FServerParameters& OutServerParameters);
};