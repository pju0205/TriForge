// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/TFMatchPlayerState.h"

#include "UI/HTTP/HTTPRequestTypes.h"

ATFMatchPlayerState::ATFMatchPlayerState()
{
	NetUpdateFrequency = 100.f; // let's not be sluggish, alright?
	
	RoundScore = 0;
	Defeats = 0;
	bWinner = false;
}

// 게임 종료시 유저 데이터 갱신시켜서 전달
void ATFMatchPlayerState::OnMatchEnded(const FString& Username)
{
	Super::OnMatchEnded(Username);

	FDSRecordMatchStatsInput RecordMatchStatsInput;
	RecordMatchStatsInput.username = Username;

	RecordMatchStatsInput.matchStats.roundScore = RoundScore;
	RecordMatchStatsInput.matchStats.defeats = Defeats;
	RecordMatchStatsInput.matchStats.matchWins = bWinner ? 1 : 0;
	RecordMatchStatsInput.matchStats.matchLosses = bWinner ? 0 : 1;
	
	RecordMatchStats(RecordMatchStatsInput);
}

// 
void ATFMatchPlayerState::AddRoundScore()
{
	++RoundScore;
}

// 패배
void ATFMatchPlayerState::AddDefeat()
{
	++Defeats;
}

// 승패 여부
void ATFMatchPlayerState::IsTheWinner()
{
	bWinner = true;
}

// 각 클라이언트에 RoundScore 변경 알리기
void ATFMatchPlayerState::Client_RoundScored_Implementation(int32 InRoundScore)
{
	OnScoreChanged.Broadcast(InRoundScore);
}


