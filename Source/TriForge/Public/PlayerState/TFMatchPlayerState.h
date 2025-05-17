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

	void AddRoundScore();
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
	int32 RoundScore;
	int32 Defeats;
	bool bWinner;
};
