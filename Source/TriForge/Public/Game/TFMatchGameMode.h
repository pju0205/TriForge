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
	virtual void BeginPlay() override;
	
	void HandleRoundEnd(APlayerController* Loser, APlayerController* Winner);
	void HandleMatchWin(APlayerController* Loser, APlayerController* Winner);

protected:
	virtual void OnCountdownTimerFinished(ECountdownTimerType Type) override;
	virtual void OnMatchEnded() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void PlayerEliminated();

	// 이동 및 정리 관련
	void PrepareNextRound();
	void NextRandomTravelMap();
	void DestroyAllDroppedWeapons();

	// 승패 관리 관련
	void HandleRoundWinner(APlayerController* Winner);
	void HandleRoundLoser(APlayerController* Loser);
	void HandleDrawRound();
	void RoundTimeOutCheck();

	UPROPERTY(EditDefaultsOnly, Category = "Maps")
	TArray<TSoftObjectPtr<UWorld>> CombatMaps;

	// 사용한 맵 목록 (SoftPath 기준으로 비교)
	UPROPERTY()
	TSet<FSoftObjectPath> UsedCombatMapPaths;

	// 최대 라운드, 매치 수
	const int32 MaxRound = 2;
	const int32 MaxMatch = 2;
	// 라운드 2승 = 매치 1승
	// 매치 2승 = 승리

	// 게임이 완전히 끝났는지
	bool bIsEndedRound = false;
	bool bIsEndedMatch = false;

private:
	FRandomStream MapRandomStream;

};
