// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TFMatchGameState.h"

#include "PlayerState/TFMatchPlayerState.h"

ATFMatchGameState::ATFMatchGameState()
{
	// 점수가 제일 높은 친구 저장
	Leaders = TArray<TObjectPtr<ATFMatchPlayerState>>();
}

ATFMatchPlayerState* ATFMatchGameState::GetLeader() const
{
	// 점수 제일 높은 애가 1명이면 그 애를 출력
	if (Leaders.Num() == 1)
	{
		return Leaders[0];
	}

	// 중복이면 아무도 안 부름
	return nullptr;
}

void ATFMatchGameState::UpdateLeader()
{
	TArray<APlayerState*> SortedPlayers = PlayerArray;
	SortedPlayers.Sort([](const APlayerState& A, const APlayerState& B)
	{
		const ATFMatchPlayerState* PlayerA = Cast<ATFMatchPlayerState>(&A);
		const ATFMatchPlayerState* PlayerB = Cast<ATFMatchPlayerState>(&B);
		return PlayerA->GetRoundScore() > PlayerB->GetRoundScore();
	});

	Leaders.Empty();
	
	if (SortedPlayers.Num() > 0)
	{
		int32 HighestScore = 0;
		for (APlayerState* PlayerState : SortedPlayers)
		{
			ATFMatchPlayerState* Player = Cast<ATFMatchPlayerState>(PlayerState);
			if (IsValid(Player))
			{
				int32 PlayerScore = Player->GetRoundScore();
                
				// On the first iteration, set the highest score
				if (Leaders.Num() == 0)
				{
					HighestScore = PlayerScore;
					Leaders.Add(Player);
				}
				else if (PlayerScore == HighestScore)
				{
					Leaders.Add(Player); // Add to leaders if scores are tied
				}
				else
				{
					break; // As it's sorted, no need to check further
				}
			}
		}
	}
}

void ATFMatchGameState::BeginPlay()
{
	Super::BeginPlay();
}

TArray<ATFMatchPlayerState*> ATFMatchGameState::GetLeaders() const
{
	return Leaders;
}
