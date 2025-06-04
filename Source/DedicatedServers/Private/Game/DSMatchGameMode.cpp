// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/DSMatchGameMode.h"

#include "Player/DSMatchPlayerState.h"
#include "Player/DSPlayerController.h"
#include "UI/GameStats/GameStatsManager.h"

ADSMatchGameMode::ADSMatchGameMode()
{
	bUseSeamlessTravel = true;

	// Match 상태 타입 초기화
	MatchStatus = EMatchStatus::WaitingForPlayers;

	// Timer 타입 초기화
	PreRoundTimer.Type = ECountdownTimerType::PreRound;
	RoundTimer.Type = ECountdownTimerType::Round;
	PostRoundTimer.Type = ECountdownTimerType::PostRound;
	PostMatchTimer.Type = ECountdownTimerType::PostMatch;
	ForceMatchEndedTimer.Type = ECountdownTimerType::ForceMatchEnded;
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
		MatchStatus = EMatchStatus::PreRound;
		StartCountdownTimer(PreRoundTimer);			// Start 카운트 다운 실행
	}
}

void ADSMatchGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	RemovePlayerSession(Exiting);
	
	const int32 RemainingPlayers = GetNumPlayers();

	// 1명 이하만 남았을 때 강제로 매치 종료
	if (RemainingPlayers <= 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Remaining player count is low. Forcing match end."));

		// 모든 타이머 정지
		StopAllCountdownTimers();
		StartCountdownTimer(ForceMatchEndedTimer);	// 강제 종료 됐을 때 실행할 타이머
	}
}

void ADSMatchGameMode::InitSeamlessTravelPlayer(AController* NewController)
{
	Super::InitSeamlessTravelPlayer(NewController);

	if (MatchStatus == EMatchStatus::WaitingForPlayers)
	{
		MatchStatus = EMatchStatus::PreRound;
		StartCountdownTimer(PreRoundTimer);
	}
}

void ADSMatchGameMode::OnCountdownTimerFinished(ECountdownTimerType Type)
{
	Super::OnCountdownTimerFinished(Type);

	// 강제로 게임 종료 됐을 때
	if (Type == ECountdownTimerType::ForceMatchEnded)
	{
		StopCountdownTimer(ForceMatchEndedTimer);
		MatchStatus = EMatchStatus::SeamlessTravelling;
		
		// 전환 직전에 LobbyMode 입력 설정
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			if (ADSPlayerController* PC = Cast<ADSPlayerController>(It->Get()))
			{
				PC->Client_SetToLobbyMode();
			}
		}
		TrySeamlessTravel(LobbyMap);
	}
}

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

void ADSMatchGameMode::StopAllCountdownTimers()
{
	StopCountdownTimer(PreRoundTimer);
	StopCountdownTimer(RoundTimer);
	StopCountdownTimer(PostRoundTimer);
	StopCountdownTimer(PostMatchTimer);
	StopCountdownTimer(ForceMatchEndedTimer);
}