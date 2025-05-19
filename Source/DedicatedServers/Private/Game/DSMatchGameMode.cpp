// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/DSMatchGameMode.h"

#include "Player/DSMatchPlayerState.h"
#include "Player/DSPlayerController.h"
#include "UI/GameStats/GameStatsManager.h"

ADSMatchGameMode::ADSMatchGameMode()
{
	bUseSeamlessTravel = true;
	MatchStatus = EMatchStatus::WaitingForPlayers;
	PreMatchTimer.Type = ECountdownTimerType::PreMatch;
	MatchTimer.Type = ECountdownTimerType::Match;
	PostMatchTimer.Type = ECountdownTimerType::PostMatch;
}

void ADSMatchGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	GameStatsManager = NewObject<UGameStatsManager>(this, GameStatsManagerClass);
	GameStatsManager->OnUpdateLeaderboardSucceeded.AddDynamic(this, &ADSMatchGameMode::ADSMatchGameMode::OnLeaderboardUpdated);
}

// 처음 들어오면서 실행
void ADSMatchGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (MatchStatus == EMatchStatus::WaitingForPlayers)
	{
		MatchStatus = EMatchStatus::PreMatch;
		StartCountdownTimer(PreMatchTimer);			// Start 카운트 다운 실행
	}
}

void ADSMatchGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	RemovePlayerSession(Exiting);
}

void ADSMatchGameMode::InitSeamlessTravelPlayer(AController* NewController)
{
	Super::InitSeamlessTravelPlayer(NewController);

	if (MatchStatus == EMatchStatus::WaitingForPlayers)
	{
		MatchStatus = EMatchStatus::PreMatch;
		StartCountdownTimer(PreMatchTimer);
	}
}

/*void ADSMatchGameMode::OnCountdownTimerFinished(ECountdownTimerType Type)
{
	Super::OnCountdownTimerFinished(Type);

	if (Type == ECountdownTimerType::PreMatch)
	{
		StopCountdownTimer(PreMatchTimer);
		MatchStatus = EMatchStatus::Match;
		StartCountdownTimer(MatchTimer);
		SetClientInputEnabled(true);			// 움직임 활성
	}
	if (Type == ECountdownTimerType::Match)
	{
		StopCountdownTimer(MatchTimer);
		MatchStatus = EMatchStatus::PostMatch;
		StartCountdownTimer(PostMatchTimer);
		SetClientInputEnabled(false);			// 움직임 비활성
		OnMatchEnded();							// 매치 종료 알림
	}
	if (Type == ECountdownTimerType::PostMatch)
	{
		StopCountdownTimer(PostMatchTimer);
		MatchStatus = EMatchStatus::SeamlessTravelling;
		TrySeamlessTravel(LobbyMap);
	}
}*/

// 움직임 활성, 비활성 함수
void ADSMatchGameMode::SetClientInputEnabled(bool bEnabled)
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		ADSPlayerController* DSPlayerController = Cast<ADSPlayerController>(Iterator->Get());
		if (IsValid(DSPlayerController))
		{
			DSPlayerController->Client_SetInputEnabled(bEnabled);
		}
	}
}

// playerState로 Match가 끝났는지 확인 작업
void ADSMatchGameMode::EndMatchForPlayerStates()
{
	// PlayerPC 하나씩 순행하며 확인
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (ADSPlayerController* DSPlayerController = Cast<ADSPlayerController>(Iterator->Get()); IsValid(DSPlayerController))
		{
			if (ADSMatchPlayerState* MatchPlayerState = DSPlayerController->GetPlayerState<ADSMatchPlayerState>(); IsValid(MatchPlayerState))
			{
				// 해당 이름을 가진 플레이어의 매치상태에서 OnMatchEnded 함수를 불렀는지 확인하여 매치 종료를 알림
				MatchPlayerState->OnMatchEnded(DSPlayerController->Username);
			}
		}
	}
}

// TFGameMode에서 오버라이딩 시켜 실행함
// 그래서 비어있음
void ADSMatchGameMode::OnMatchEnded()
{
	
}

void ADSMatchGameMode::UpdateLeaderboard(const TArray<FString>& LeaderboardNames)
{
	if (IsValid(GameStatsManager))
	{
		GameStatsManager->UpdateLeaderboard(LeaderboardNames);
	}
}

void ADSMatchGameMode::OnLeaderboardUpdated()
{
	EndMatchForPlayerStates();
}