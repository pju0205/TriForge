// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DSMatchPlayerState.h"

#include "UI/GameStats/GameStatsManager.h"

// Match 끝났는지 알림 용 함수
// 자식 함수(TFMatchPlayerState)에서 오버라이딩됨
void ADSMatchPlayerState::OnMatchEnded(const FString& Username)
{
}

void ADSMatchPlayerState::BeginPlay()
{
	Super::BeginPlay();
	GameStatsManager = NewObject<UGameStatsManager>(this, GameStatsManagerClass);
}

void ADSMatchPlayerState::RecordMatchStats(const FDSRecordMatchStatsInput& RecordMatchStatsInput) const
{
	check(IsValid(GameStatsManager));
	GameStatsManager->RecordMatchStats(RecordMatchStatsInput);
}