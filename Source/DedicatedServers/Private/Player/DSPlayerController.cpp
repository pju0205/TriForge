// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DSPlayerController.h"


ADSPlayerController::ADSPlayerController()
{
	SingleTripTime = 0.f;
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