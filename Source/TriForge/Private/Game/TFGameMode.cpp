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

	bIsEndedMatch = false;
}

void ATFGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATFGameMode::HandleRoundEnd(APlayerController* Loser, APlayerController* Winner)
{
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
		HandleMatchWin(Winner);
	}
	else
	{
		StopCountdownTimer(RoundTimer);
		MatchStatus = EMatchStatus::PostRound;
		StartCountdownTimer(PostRoundTimer);
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
	StartCountdownTimer(PreRoundTimer);
}

void ATFGameMode::HandleMatchWin(APlayerController* Winner)
{
	ATFMatchPlayerState* WinnerPS = Winner ? Cast<ATFMatchPlayerState>(Winner->PlayerState) : nullptr;
	if (WinnerPS)
	{
		WinnerPS->MatchWins++;		// Local 매치 횟수 1 증가
		WinnerPS->AddMatchScore();	// PlayerState 값 증가 (기록용)
	}

	// MatchWin가 2회 이상이면
	if (WinnerPS && WinnerPS->MatchWins >= MaxMatch)
	{
		bIsEndedMatch = true;
		UE_LOG(LogTemp, Display, TEXT("bIsEndedMatch = true"));
	}
	
	// MaxMatch 횟수 다 채웠으면 Winner 승리로 간주
	if (bIsEndedMatch)
	{
		// 완전 종료 게임 끝
		WinnerPS->IsTheWinner();				// 승리자로 기록
		/* OnMatchEnded();						// 이게 모두한테 실행이 되나? 일단 보류 */
		
		StopCountdownTimer(RoundTimer);
		MatchStatus = EMatchStatus::GameOver;
		StartCountdownTimer(ForceMatchEndedTimer);
	}
	else
	{
		StopCountdownTimer(RoundTimer);
		MatchStatus = EMatchStatus::PostMatch;
		StartCountdownTimer(PostMatchTimer);
	}
}

void ATFGameMode::OnCountdownTimerFinished(ECountdownTimerType Type)
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
		NextRandomTravelMap();							// 다음 전투 맵 이동
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

void ATFGameMode::NextRandomTravelMap()
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
