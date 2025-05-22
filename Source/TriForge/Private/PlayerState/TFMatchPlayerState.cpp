// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/TFMatchPlayerState.h"

#include "Character/TFPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Types/TFTypes.h"
#include "UI/HTTP/HTTPRequestTypes.h"

ATFMatchPlayerState::ATFMatchPlayerState()
{
	NetUpdateFrequency = 100.f; // let's not be sluggish, alright?

	Hits	= 0;
	Misses = 0;
	Kills = 0;
	Deaths = 0;
	RoundScore = 0;
	MatchScore = 0;
	Defeats = 0;
	bWinner = false;

	RoundWins = 0;
	MatchWins = 0;
}

// 게임 종료시 유저 데이터 갱신시켜서 전달
void ATFMatchPlayerState::OnMatchEnded(const FString& Username)
{
	Super::OnMatchEnded(Username);

	FDSRecordMatchStatsInput RecordMatchStatsInput;
	RecordMatchStatsInput.username = Username;

	// Hit, miss, kill, death 넣어야됨
	RecordMatchStatsInput.matchStats.roundScore = RoundScore;
	RecordMatchStatsInput.matchStats.defeats = Defeats;
	RecordMatchStatsInput.matchStats.matchWins = bWinner ? 1 : 0;
	RecordMatchStatsInput.matchStats.matchLosses = bWinner ? 0 : 1;
	
	RecordMatchStats(RecordMatchStatsInput);
}

// Seamless Travel시 복사시킬 데이터
void ATFMatchPlayerState::CopyProperties(APlayerState* NewPlayerState)
{
	Super::CopyProperties(NewPlayerState);

	if (ATFMatchPlayerState* NewTFPS = Cast<ATFMatchPlayerState>(NewPlayerState))
	{
		NewTFPS->Hits = this->Hits;
		NewTFPS->Misses = this->Misses;
		NewTFPS->Kills = this->Kills;
		NewTFPS->Deaths = this->Deaths;
		NewTFPS->RoundScore = this->RoundScore;
		NewTFPS->MatchScore = this->MatchScore;
		NewTFPS->Defeats = this->Defeats;
		NewTFPS->bWinner = this->bWinner;

		NewTFPS->MatchWins = this->MatchWins;
		// RoundWins 는 미포함
	}
}

// 맞춘 횟수
void ATFMatchPlayerState::AddHit()
{
	++Hits;
}

// 빗맞춘 횟수
void ATFMatchPlayerState::AddMiss()
{
	++Misses;
}

// 킬 횟수
void ATFMatchPlayerState::AddKill()
{
	++Kills;
}

// 데스 횟수
void ATFMatchPlayerState::AddDeath()
{
	++Deaths;

	const FString Name = GetPlayerName(); // APlayerState에서 제공하는 함수

	UE_LOG(LogTemp, Log, TEXT("Player %s AddDeath Total Deaths : %d"), *Name, Deaths);
}

// 라운드 승리 횟수 
void ATFMatchPlayerState::AddRoundScore()
{
	++RoundScore;
	
	const FString Name = GetPlayerName(); // APlayerState에서 제공하는 함수

	UE_LOG(LogTemp, Log, TEXT("Player %s gained a round win! Total Round Wins: %d"), *Name, RoundWins);
	UE_LOG(LogTemp, Log, TEXT("Player %s gained a round win! Total Round Score: %d"), *Name, RoundScore);
	
}

// 매치 승리 횟수
void ATFMatchPlayerState::AddMatchScore()
{
	++MatchScore;
	
	const FString Name = GetPlayerName();
	UE_LOG(LogTemp, Log, TEXT("Player %s gained a Match win! Total Match Wins: %d"), *Name, MatchWins);
	UE_LOG(LogTemp, Log, TEXT("Player %s gained a Match win! Total Match Score: %d"), *Name, MatchScore);
}

// 승패 여부
void ATFMatchPlayerState::IsTheWinner()
{
	bWinner = true;
}


void ATFMatchPlayerState::AddRoundResult(bool bWon)
{
	RoundResults.Add(bWon);

	if (HasAuthority())
	{
		OnRep_RoundResults(); // 서버에서도 수동 호출 (서버용 UI도 있다면)
	}
}

void ATFMatchPlayerState::OnRep_RoundResults()
{
	if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
	{
		if (ATFPlayerController* TFPC = Cast<ATFPlayerController>(PC))
		{
			TFPC->UpdateRoundIndicator();
		}
	}
}

// 각 클라이언트에 RoundScore 변경 알리기
void ATFMatchPlayerState::Client_RoundScored_Implementation(int32 InRoundScore)
{
	OnScoreChanged.Broadcast(InRoundScore);
}

void ATFMatchPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATFMatchPlayerState, RoundResults);
}
