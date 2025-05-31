// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TFMatchGameMode.h"

#include "Character/TFPlayerController.h"
#include "Game/TFMatchGameState.h"
#include "GameFramework/GameStateBase.h"
#include "Player/DSPlayerController.h"
#include "PlayerState/TFMatchPlayerState.h"

ATFMatchGameMode::ATFMatchGameMode()
{
	bUseSeamlessTravel = true;

	bIsEndedMatch = false;
}

void ATFMatchGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATFMatchGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void ATFMatchGameMode::HandleRoundEnd(APlayerController* Loser, APlayerController* Winner)
{
	if (MatchStatus != EMatchStatus::Round) return;
	
	// Winner가 null이면 생존자 찾아서 할당 (낙사와 같은 경우)
	if (!Winner)
	{
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			APlayerController* PC = It->Get();
			if (!IsValid(PC) || PC == Loser) continue;

			ATFPlayerController* TFPC = Cast<ATFPlayerController>(PC);
			if (IsValid(TFPC) && TFPC->bPawnAlive)
			{
				Winner = TFPC;
				break;
			}
		}
	}
	
	ATFMatchPlayerState* WinnerPS = Winner ? Cast<ATFMatchPlayerState>(Winner->PlayerState) : nullptr;
	ATFMatchPlayerState* LoserPS = Loser ? Cast<ATFMatchPlayerState>(Loser->PlayerState) : nullptr;

	if (WinnerPS)
	{
		WinnerPS->RoundWins++;		// Local 라운드 횟수 1 증가
		WinnerPS->AddRoundScore();	// PlayerState 값 증가 (기록용)
		WinnerPS->AddKill();
		WinnerPS->AddRoundResult(true);
	}
	if (LoserPS)
	{
		LoserPS->AddDeath();
		LoserPS->AddRoundResult(false);
	}
	
	// Winner 라운드 승리 횟수 확인
	if (WinnerPS && WinnerPS->RoundWins >= MaxRound)	// 2번 이상 라운드 승리시
	{
		HandleMatchWin(Loser, Winner);
		bIsEndedRound = true;
	}
	else
	{
		StopCountdownTimer(RoundTimer);
		MatchStatus = EMatchStatus::PostRound;
		StartCountdownTimer(PostRoundTimer);
	}
}

void ATFMatchGameMode::HandleMatchWin(APlayerController* Loser, APlayerController* Winner)
{
	ATFMatchPlayerState* WinnerPS = Winner ? Cast<ATFMatchPlayerState>(Winner->PlayerState) : nullptr;
	ATFMatchPlayerState* LoserPS = Loser ? Cast<ATFMatchPlayerState>(Loser->PlayerState) : nullptr;
	
	if (WinnerPS)
	{
		WinnerPS->AddMatchScore();	// PlayerState 값 증가 (기록용)
		WinnerPS->AddMatchResult(true);
	}

	if (LoserPS)
	{
		LoserPS->AddMatchResult(false);
	}

	// MatchWin가 2회 이상이면
	if (WinnerPS && WinnerPS->MyMatchWins >= MaxMatch)
	{
		bIsEndedMatch = true;
	}
	
	// MaxMatch 횟수 다 채웠으면 Winner 승리로 간주
	if (bIsEndedMatch)
	{
		ATFMatchGameState* GS = GetGameState<ATFMatchGameState>();
		if (IsValid(GS))
		{
			GS->UpdateLeader(); // 최종 승자 업데이트
		}
		
		// 완전 종료 게임 끝
		WinnerPS->IsTheWinner();				// 승리자로 기록
		OnMatchEnded();						// 이게 모두한테 실행이 되나? 일단 보류
		
		StopCountdownTimer(RoundTimer);
		MatchStatus = EMatchStatus::GameOver;
		StartCountdownTimer(ForceMatchEndedTimer);
	}
	else
	{
		bIsEndedRound = false;
		StopCountdownTimer(RoundTimer);
		MatchStatus = EMatchStatus::PostMatch;
		StartCountdownTimer(PostMatchTimer);
	}
}

void ATFMatchGameMode::OnCountdownTimerFinished(ECountdownTimerType Type)
{
	Super::OnCountdownTimerFinished(Type);

	// Round 관련
	if (Type == ECountdownTimerType::PreRound)
	{
		StopCountdownTimer(PreRoundTimer);
		MatchStatus = EMatchStatus::Round;
		StartCountdownTimer(RoundTimer);
		SetClientInputEnabled(true);
	}
	if (Type == ECountdownTimerType::Round)
	{
		MatchStatus = EMatchStatus::PostRound;
		StartCountdownTimer(PostRoundTimer);
		SetClientInputEnabled(false);
	}
	if (Type == ECountdownTimerType::PostRound)
	{
		StopCountdownTimer(PostRoundTimer);
		MatchStatus = EMatchStatus::PreRound;
		SetClientInputEnabled(false);
		PrepareNextRound();								// 다음 라운드 준비
	}
	
	// Match 관련
	if (Type == ECountdownTimerType::PostMatch)			// Round 2번 승리 시 실행 타이머
	{
		StopCountdownTimer(PostMatchTimer);
		MatchStatus = EMatchStatus::SeamlessTravelling;
		PlayerEliminated();
		NextRandomTravelMap();							// 다음 전투 맵 이동
	}
}

// 다음 라운드 넘어 갈 때 (기존 맵 유지, Pawn만 새로 생성, Timer 새로 시작)
void ATFMatchGameMode::PrepareNextRound()
{
	// player 삭제해야 되는 데이터 삭제
	PlayerEliminated();

	// 라운드 타이머 시작
	StartCountdownTimer(PreRoundTimer);
}

// TFGameMode에서 실행
void ATFMatchGameMode::PlayerEliminated()
{
	
}

void ATFMatchGameMode::NextRandomTravelMap()
{
	// 랜덤 맵 선택
	if (CombatMaps.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, CombatMaps.Num() - 1);
		
		TSoftObjectPtr<UWorld> SelectedMap = CombatMaps[RandomIndex];
		if (SelectedMap.IsValid() || SelectedMap.ToSoftObjectPath().IsValid())
		{
			// 선택된 맵으로 이동
			TrySeamlessTravel(SelectedMap);

			// Map 이동 로그 출력
			UE_LOG(LogTemp, Display, TEXT("Map Index: %d, Path: %s"), RandomIndex, *SelectedMap.ToString());
		}
	}
}

void ATFMatchGameMode::GetGamePlayerName()
{
	TArray<ADSPlayerController*> Controllers;
    
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ADSPlayerController* PC = Cast<ADSPlayerController>(It->Get());
		if (PC)
		{
			Controllers.Add(PC);
		}
	}

	// 두 명이 접속한 경우, 서로의 OpponentUsername 설정
	if (Controllers.Num() == 2)
	{
		ADSPlayerController* PC1 = Controllers[0];
		ADSPlayerController* PC2 = Controllers[1];

		if (PC1 && PC2)
		{
			if (PC1->Username != PC2->Username)
			{
				PC1->OpponentUsername = PC2->Username;
				PC2->OpponentUsername = PC1->Username;

				UE_LOG(LogTemp, Warning, TEXT("PC1(%s) Opponent: %s"), *PC1->Username, *PC1->OpponentUsername);
				UE_LOG(LogTemp, Warning, TEXT("PC2(%s) Opponent: %s"), *PC2->Username, *PC2->OpponentUsername);
			}
			// 로그 확인용
		}
	}
}
