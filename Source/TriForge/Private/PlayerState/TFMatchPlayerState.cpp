// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/TFMatchPlayerState.h"

#include "Character/TFPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "UI/HTTP/HTTPRequestTypes.h"

ATFMatchPlayerState::ATFMatchPlayerState()
{
	NetUpdateFrequency = 100.f; // let's not be sluggish, alright?

	// 기록 용 변수들
	// Hits	= 0;
	// Misses = 0;
	Kills = 0;
	Deaths = 0;
	RoundScore = 0;
	MatchScore = 0;
	bWinner = false;

	// 로컬 변수들
	RoundWins = 0;
	MyMatchWins = 0;
	OpponentMatchWins = 0;
	MatchResults = false;
}

// 게임 종료시 유저 데이터 갱신시켜서 전달
void ATFMatchPlayerState::OnMatchEnded(const FString& Username)
{
	Super::OnMatchEnded(Username);

	FDSRecordMatchStatsInput RecordMatchStatsInput;
	RecordMatchStatsInput.username = Username;

	// Hit, miss, kill, death 넣어야됨
	RecordMatchStatsInput.matchStats.kills = Kills;
	RecordMatchStatsInput.matchStats.deaths = Deaths;
	RecordMatchStatsInput.matchStats.roundScore = RoundScore;
	RecordMatchStatsInput.matchStats.matchScore = MatchScore;
	RecordMatchStatsInput.matchStats.matchWins = bWinner ? 1 : 0;
	RecordMatchStatsInput.matchStats.matchLosses = bWinner ? 0 : 1;
	
	RecordMatchStats(RecordMatchStatsInput);
}

// Seamless Travel시 복사시킬 데이터
void ATFMatchPlayerState::CopyProperties(APlayerState* NewPlayerState)
{
	Super::CopyProperties(NewPlayerState);

	if (ATFMatchPlayerState* TFPS = Cast<ATFMatchPlayerState>(NewPlayerState))
	{
		TFPS->Kills = this->Kills;
		TFPS->Deaths = this->Deaths;
		TFPS->RoundScore = this->RoundScore;
		TFPS->MatchScore = this->MatchScore;
		TFPS->bWinner = this->bWinner;

		TFPS->MyMatchWins = this->MyMatchWins;
		TFPS->OpponentMatchWins = this->OpponentMatchWins;
		// RoundWins 미포함
	}
}

// // 맞춘 횟수
// void ATFMatchPlayerState::AddHit()
// {
// 	++Hits;
// }
//
// // 빗맞춘 횟수
// void ATFMatchPlayerState::AddMiss()
// {
// 	++Misses;
// }

// 킬 횟수
void ATFMatchPlayerState::AddKill()
{
	++Kills;
}

// 데스 횟수
void ATFMatchPlayerState::AddDeath()
{
	++Deaths;
}

// 라운드 승리 횟수 
void ATFMatchPlayerState::AddRoundScore()
{
	++RoundScore;
}

// 매치 승리 횟수
void ATFMatchPlayerState::AddMatchScore()
{
	++MatchScore;
}

// 승패 여부
void ATFMatchPlayerState::IsTheWinner()
{
	bWinner = true;
}


// ㅡㅡㅡㅡㅡㅡㅡㅡㅡ 라운드 관련 ㅡㅡㅡㅡㅡㅡㅡㅡㅡ //
void ATFMatchPlayerState::AddRoundResult(bool bWon)
{
	RoundResults.Add(bWon);

	Client_RoundResult(RoundResults);
}

void ATFMatchPlayerState::Client_RoundResult_Implementation(const TArray<bool>& InRoundResults)
{
	OnRoundResultChanged.Broadcast(InRoundResults);
}

// ㅡㅡㅡㅡㅡㅡㅡㅡㅡ Match 관련 시작 ㅡㅡㅡㅡㅡㅡㅡㅡㅡ //
void ATFMatchPlayerState::AddMatchResult(bool bWon)
{
	MatchResults = bWon;

	if (bWon) MyMatchWins++;
	else OpponentMatchWins++;

	Client_MatchResult(MatchResults, MyMatchWins, OpponentMatchWins);
}

void ATFMatchPlayerState::Client_MatchResult_Implementation(bool bWon, int32 MyScore, int32 OpponentScore)
{
	OnMatchResultChanged.Broadcast(bWon, MyScore, OpponentScore);
}

// 값 멀티 처리
void ATFMatchPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATFMatchPlayerState, MatchResults);
	DOREPLIFETIME(ATFMatchPlayerState, RoundResults);
	DOREPLIFETIME(ATFMatchPlayerState, MyMatchWins);
	DOREPLIFETIME(ATFMatchPlayerState, OpponentMatchWins);
}

void ATFMatchPlayerState::OnRep_MatchResults(bool bWon)
{
}

void ATFMatchPlayerState::OnRep_RoundResults()
{
}


