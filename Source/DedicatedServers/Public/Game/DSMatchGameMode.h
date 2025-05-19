// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DSGameModeBase.h"
#include "DSMatchGameMode.generated.h"

class UGameStatsManager;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API ADSMatchGameMode : public ADSGameModeBase
{
	GENERATED_BODY()

public:
	ADSMatchGameMode();
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	virtual void InitSeamlessTravelPlayer(AController* NewController) override;
 
	UPROPERTY()
	EMatchStatus MatchStatus;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameStatsManager> GameStatsManagerClass;
 
protected:
	/*virtual void OnCountdownTimerFinished(ECountdownTimerType Type) override;*/

	virtual void BeginPlay() override;
	void SetClientInputEnabled(bool bEnabled);
	void EndMatchForPlayerStates();
	virtual void OnMatchEnded();
	void UpdateLeaderboard(const TArray<FString>& LeaderboardNames);
	
	UPROPERTY(EditDefaultsOnly)
	FCountdownTimerHandle PreMatchTimer;
 
	UPROPERTY(EditDefaultsOnly)
	FCountdownTimerHandle MatchTimer;
 
	UPROPERTY(EditDefaultsOnly)
	FCountdownTimerHandle PostMatchTimer;

	UPROPERTY(EditDefaultsOnly)
	FCountdownTimerHandle MatchEndedTimer;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> LobbyMap;

	UFUNCTION()
	void OnLeaderboardUpdated();

private:
	UPROPERTY()
	TObjectPtr<UGameStatsManager> GameStatsManager;
};
