// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DSPlayerController.h"

#include "Game/DSGameState.h"
#include "Game/DSLobbyGameMode.h"
#include "Lobby/LobbyState.h"


ADSPlayerController::ADSPlayerController()
{
	SingleTripTime = 0.f;
	Username = "";
	PlayerSessionId = "";
}

// 이 구조는 네트워크 상에서 RTT (Round Trip Time) 를 측정하기 위한 가장 간단한 방식
// 클라이언트가 서버에 접속 완료되었을 때 호출
void ADSPlayerController::ReceivedPlayer()
{
 	Super::ReceivedPlayer();
	
 	if (GetNetMode() == NM_Standalone) return;
	
 	if (IsLocalController())
 	{
 		Server_Ping(GetWorld()->GetTimeSeconds());
 	}
}

// ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ Input Setting 해결 함수
void ADSPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
 
	if (IsLocalController())
	{
		DisableInput(this);
	}
}
 
void ADSPlayerController::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();
 
	if (IsLocalController())
	{
		Server_Ping(GetWorld()->GetTimeSeconds());
		DisableInput(this);
	}
}
 
void ADSPlayerController::BeginPlay()
{
	Super::BeginPlay();
 
	/*if (GetNetMode() == NM_Standalone)
	{
		DisableInput(this);
	}*/
}

// Game 진입 시 Input 설정
void ADSPlayerController::Client_SetToGameMode_Implementation()
{
	bShowMouseCursor = false;
	bEnableClickEvents = false;
	bEnableMouseOverEvents = false;

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}

// Lobby 진입 시 Input 설정
void ADSPlayerController::Client_SetToLobbyMode_Implementation()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetWidgetToFocus(nullptr); // UI 포커스 줄 수 있으면 넣기
	SetInputMode(InputMode);
}

void ADSPlayerController::Server_SetReadyState_Implementation(bool bNewReady)
{
	if (ADSGameState* GS = GetWorld()->GetGameState<ADSGameState>())
	{
		if (ALobbyState* LobbyState = GS->LobbyState)
		{
			FString Playername = this->Username;
			LobbyState->SetPlayerReadyState(Playername, bNewReady);			// Player Name이 일치하는 유저 찾아서 Ready 상태 변경

			// 모든 플레이어가 준비 완료되었는지 체크
			if (ADSLobbyGameMode* GM = GetWorld()->GetAuthGameMode<ADSLobbyGameMode>())
			{
				GM->CheckAndStartLobbyCountdown();
			}
		}
	}
}

void ADSPlayerController::Client_SetInputEnabled_Implementation(bool bEnabled)
{
	if (bEnabled)
	{
		EnableInput(this);
	}
	else
	{
		DisableInput(this);
	}
}
// ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ Input Setting 해결 함수

void ADSPlayerController::Client_TimerUpdated_Implementation(float CountdownTimeLeft, ECountdownTimerType Type) const
{
	// 해당 함수를 통해 서버가 클라이언트에게 타이머 상태 알려줌
	// CountdownTimeLeft: 서버 기준 남은 시간
	// SingleTripTime: 클라이언트가 Ping/Pong을 통해 계산한 네트워크 지연 시간
	OnTimerUpdated.Broadcast(CountdownTimeLeft - SingleTripTime, Type);
}
 
void ADSPlayerController::Client_TimerStopped_Implementation(float CountdownTimeLeft, ECountdownTimerType Type) const
{
	// 타이머가 멈췄다는 신호를 서버에서 클라이언트에게 전달
	OnTimerStopped.Broadcast(CountdownTimeLeft - SingleTripTime, Type);
}
 

void ADSPlayerController::Server_Ping_Implementation(float TimeOfRequest)
{
 	Client_Pong(TimeOfRequest);	// 전달받은 TimeOfRequest를 그대로 클라이언트에게 다시 보냄
}


void ADSPlayerController::Client_Pong_Implementation(float TimeOfRequest)
{
 	const float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfRequest;		// 현재 시간에서 Ping을 보낼 때의 시간을 빼서 RTT를 구함
 	SingleTripTime = RoundTripTime * 0.5f;											// RTT의 절반 = 단방향 지연 시간, 즉 SingleTripTime
}
