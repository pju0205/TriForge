// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/DSMatchGameMode.h"
#include "TFMatchGameMode.generated.h"

// 전체적인 게임 흐름 관리하는 GameMode
/**
 * 
 */
UCLASS()
class TRIFORGE_API ATFMatchGameMode : public ADSMatchGameMode
{
	GENERATED_BODY()
public:
	ATFMatchGameMode();
	virtual void Tick(float DeltaTime) override;
	
	void HandleRoundEnd(APlayerController* Loser, APlayerController* Winner);
	void HandleMatchWin(APlayerController* Loser, APlayerController* Winner);

protected:
	virtual void OnCountdownTimerFinished(ECountdownTimerType Type) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	virtual void PlayerEliminated();
	void PrepareNextRound();
	void NextRandomTravelMap();
	void DestroyAllDroppedWeapons();

	UPROPERTY(EditDefaultsOnly, Category = "Maps")
	TArray<TSoftObjectPtr<UWorld>> CombatMaps;

	// 최대 라운드, 매치 수
	const int32 MaxRound = 2;
	const int32 MaxMatch = 2;
	// 라운드 2승 = 매치 1승
	// 매치 2승 = 승리

	// 게임이 완전히 끝났는지
	bool bIsEndedRound = false;
	bool bIsEndedMatch = false;

private:
	void GetGamePlayerName();	
};
