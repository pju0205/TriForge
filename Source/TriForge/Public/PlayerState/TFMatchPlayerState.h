// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/DSMatchPlayerState.h"
#include "TFMatchPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreChanged, int32, NewScore);
class UTFPlayerData;
/**
 * 
 */
UCLASS()
class TRIFORGE_API ATFMatchPlayerState : public ADSMatchPlayerState
{
	GENERATED_BODY()

public:
	ATFMatchPlayerState();
	virtual void OnMatchEnded(const FString& Username) override;
	virtual void CopyProperties(APlayerState* NewPlayerState) override;
	

	void AddHit();
	void AddMiss();
	void AddKill();
	void AddDeath();
	void AddRoundScore();
	void AddMatchScore();
	void AddDefeat();
	void IsTheWinner();
	int32 GetRoundScore() const { return RoundScore; };
	
	UFUNCTION(Client, Reliable)
	void Client_RoundScored(int32 InRoundScore);

	UPROPERTY(BlueprintAssignable)
	FOnScoreChanged OnScoreChanged;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTFPlayerData> TFPlayerData;

private:
	// 기록 할 데이터
	int32 Hits;
	int32 Misses;
	int32 Kills;
	int32 Deaths;
	int32 RoundScore;
	int32 MatchScore;
	int32 Defeats;
	bool bWinner;
public:
	int32 RoundWins;
	int32 MatchWins;
};
