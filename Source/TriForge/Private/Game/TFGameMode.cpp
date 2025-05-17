// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TFGameMode.h"

#include "Game/TFMatchGameState.h"
#include "Player/DSPlayerController.h"
#include "PlayerState/TFMatchPlayerState.h"

void ATFGameMode::OnMatchEnded()
{
	Super::OnMatchEnded();

	TArray<FString> LeaderIds;
	if (ATFMatchGameState* MatchGameState = GetGameState<ATFMatchGameState>(); IsValid(MatchGameState))
	{
		TArray<ATFMatchPlayerState*> Leaders = MatchGameState->GetLeaders();
		for (ATFMatchPlayerState* Leader : Leaders)
		{
			if (ADSPlayerController* LeaderPC = Cast<ADSPlayerController>(Leader->GetPlayerController()); IsValid(LeaderPC))
			{
				LeaderIds.Add(LeaderPC->Username);
			}
		}
	}
	UpdateLeaderboard(LeaderIds);
}
