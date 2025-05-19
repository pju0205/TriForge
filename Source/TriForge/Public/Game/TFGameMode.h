// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/DSMatchGameMode.h"
#include "TFGameMode.generated.h"

// 전체적인 게임 흐름 관리하는 GameMode
/**
 * 
 */
UCLASS()
class TRIFORGE_API ATFGameMode : public ADSMatchGameMode
{
	GENERATED_BODY()
public:
	ATFGameMode();
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	TMap<APlayerController*, FTimerHandle> Timers;
	
	virtual void PlayerEliminated(ACharacter* ElimmedCharacter, class APlayerController* VictimController, APlayerController* AttackerController);

	void HandleRoundEnd(APlayerController* Loser, APlayerController* Winner);
	void HandleMatchWin(APlayerController* Winner);
protected:
	virtual void OnCountdownTimerFinished(ECountdownTimerType Type) override;
	
	virtual void OnMatchEnded() override;
	
	void PrepareNextRound();
	void RandomTravelMap();

	UPROPERTY(EditDefaultsOnly, Category = "Maps")
	TArray<TSoftObjectPtr<UWorld>> CombatMaps;

	// 최대 라운드, 매치 수
	int32 CurrentRound = 0;
	int32 CurrentMatch = 0;
	
	int32 MaxRound;
	int32 MaxMatch;
	// 라운드 2승 = 매치 1승
	// 매치 2승 = 승리

	// 게임이 완전히 끝났는지
	bool bIsEndedGame = false;
};
