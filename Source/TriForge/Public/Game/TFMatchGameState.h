// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "TFMatchGameState.generated.h"


class ATFMatchPlayerState;
/**
 * 
 */
UCLASS()
class TRIFORGE_API ATFMatchGameState : public AGameState
{
	GENERATED_BODY()

public:
	ATFMatchGameState();
	ATFMatchPlayerState* GetLeader() const;

	void UpdateLeader();
	TArray<ATFMatchPlayerState*> GetLeaders() const;

	int32 CurrentRound = 0;
	const int32 MaxRound = 3;
protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY()
	TArray<TObjectPtr<ATFMatchPlayerState>> Leaders;

	UPROPERTY()
	TArray<TObjectPtr<ATFMatchPlayerState>> SortedPlayerStates;
};