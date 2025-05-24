// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TFGameMode.h"

#include "Game/TFMatchGameState.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Player/DSPlayerController.h"
#include "PlayerState/TFMatchPlayerState.h"

ATFGameMode::ATFGameMode()
{
	bUseSeamlessTravel = true;
}

void ATFGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATFGameMode::PlayerEliminated()
{
	Super::PlayerEliminated();

	// Round 시작마다 Start 지점 사용한 캐릭터 비워주기
	UsedPlayerStarts.Empty();

	// 게임 중인 전체 컨트롤러 찾기
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC)
		{
			// 현재 있는 캐릭터들 전부 초기화
			APawn* Pawn = PC->GetPawn();
			if (Pawn)
			{
				PC->UnPossess();
				Pawn->Destroy();
			}

			// 게임이 끝난게 아니라면 리스폰
			if (!bIsEndedMatch && !bIsEndedRound)
			{
				AActor* StartSpot = ChoosePlayerStart(PC); // 내부에서 중복 피할 수 있음
				RestartPlayerAtPlayerStart(PC, StartSpot);
			}
		}
	}
}

AActor* ATFGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);

	// 사용되지 않은 PlayerStart 검색
	for (AActor* Start : PlayerStarts)
	{
		if (!UsedPlayerStarts.Contains(Start))
		{
			UsedPlayerStarts.Add(Start);
			return Start;
		}
	}

	// 모두 사용되었으면 기본 로직으로 fallback
	return Super::ChoosePlayerStart_Implementation(Player);
}

void ATFGameMode::OnCountdownTimerFinished(ECountdownTimerType Type)
{
	Super::OnCountdownTimerFinished(Type);

	if (Type == ECountdownTimerType::ForceMatchEnded)
	{
		// 게임 종료시 Player 삭제
		PlayerEliminated();
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
