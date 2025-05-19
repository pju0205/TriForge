// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TFGameMode.h"

#include "Game/TFMatchGameState.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Player/DSPlayerController.h"
#include "PlayerState/TFMatchPlayerState.h"

ATFGameMode::ATFGameMode()
{
	bUseSeamlessTravel = true;
	
	CurrentRound = 0;
	CurrentMatch = 0;
	
	MaxRound = 2;
	MaxMatch = 3;

	bIsEndedGame = false;
}

void ATFGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// 플레이어 죽었을 때 실행할 함수
void ATFGameMode::PlayerEliminated(ACharacter* ElimmedCharacter, class APlayerController* VictimController,
	APlayerController* AttackerController)
{
	// 라운드 종료 로직 호출
	HandleRoundEnd(VictimController, AttackerController);
}

void ATFGameMode::HandleRoundEnd(APlayerController* Loser, APlayerController* Winner)
{
	ATFMatchPlayerState* WinnerPS = Winner ? Cast<ATFMatchPlayerState>(Winner->PlayerState) : nullptr;
	ATFMatchPlayerState* LoserPS = Loser ? Cast<ATFMatchPlayerState>(Loser->PlayerState) : nullptr;

	if (WinnerPS)
	{
		++CurrentRound;
		WinnerPS->RoundWin++;
		WinnerPS->AddRoundScore();	// PlayerState 값 증가
	}
	
	// Winner 라운드 승리 횟수 확인
	if (WinnerPS && WinnerPS->RoundWin >= MaxRound)	// 2번 이상 승리시
	{
		HandleMatchWin(Winner);
	}
	else
	{
		StopCountdownTimer(MatchTimer);
	}
}

// 다음 라운드 넘어 갈 때 (기존 맵 유지, Pawn만 새로 생성, Timer 새로 시작)
void ATFGameMode::PrepareNextRound()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC)
		{
			APawn* Pawn = PC->GetPawn();
			if (Pawn)
			{
				Pawn->Reset();
				Pawn->Destroy();	// 기존 Pawn 제거
			}

			// 리스폰 (새 Pawn 생성)
			TArray<AActor*> PlayerStarts;
			UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
			if (PlayerStarts.Num() > 0)
			{
				int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
				RestartPlayerAtPlayerStart(PC, PlayerStarts[Selection]);
			}
		}
	}

	// 라운드 타이머 시작
	StartCountdownTimer(PreMatchTimer);
}

void ATFGameMode::HandleMatchWin(APlayerController* Winner)
{
	ATFMatchPlayerState* WinnerPS = Winner ? Cast<ATFMatchPlayerState>(Winner->PlayerState) : nullptr;
	if (WinnerPS)
	{
		++CurrentMatch;
		WinnerPS->MatchWin++;		// 전체 매치 점수 1점 획득
		WinnerPS->AddMatchScore();	// PlayerState 값 증가
	}

	// 모든 PlayerState 순회하며 MaxMatch 달성자 찾기
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (IsValid(PC))
		{
			ATFMatchPlayerState* PS = Cast<ATFMatchPlayerState>(PC->PlayerState);
			if (IsValid(PS) && PS->MatchWin >= MaxMatch)
			{
				bIsEndedGame = true;
			}
		}
	}
	
	// MaxMatch 횟수 다 채웠으면 게임 끝
	if (bIsEndedGame)
	{
		// 완전 종료 게임 끝
		WinnerPS->IsTheWinner();				// 승리자로 기록
		StopCountdownTimer(MatchTimer);
	}
	else
	{
		// 다음 맵으로 전환
		RandomTravelMap();
	}
}

void ATFGameMode::OnCountdownTimerFinished(ECountdownTimerType Type)
{
	Super::OnCountdownTimerFinished(Type);

	if (Type == ECountdownTimerType::PreMatch)
	{
		StopCountdownTimer(PreMatchTimer);
		MatchStatus = EMatchStatus::Match;
		StartCountdownTimer(MatchTimer);
		SetClientInputEnabled(true);
	}
	if (Type == ECountdownTimerType::Match)
	{
		MatchStatus = EMatchStatus::PostMatch;
		StartCountdownTimer(PostMatchTimer);
		SetClientInputEnabled(false);
	}
	if (Type == ECountdownTimerType::PostMatch)
	{
		if (bIsEndedGame)									// Match가 완전히 끝나면
		{
			MatchStatus = EMatchStatus::WaitingForPlayers;
			StartCountdownTimer(MatchEndedTimer);
			SetClientInputEnabled(false);
		}
		else
		{
			StopCountdownTimer(PostMatchTimer);
			MatchStatus = EMatchStatus::WaitingForPlayers;
			SetClientInputEnabled(false);
			PrepareNextRound();
		}
	}
	if (Type == ECountdownTimerType::MatchEndedCountdown)	// 게임 종료용 타이머
	{
		StopCountdownTimer(MatchEndedTimer);
		MatchStatus = EMatchStatus::SeamlessTravelling;
		TrySeamlessTravel(LobbyMap);
	}
}

// 게임 완전히 끝났을 때
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

void ATFGameMode::RandomTravelMap()
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
		}
	}
}
