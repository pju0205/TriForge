// Fill out your copyright notice in the Description page of Project Settings.
 
 
#include "Game/DSGameState.h"
#include "Net/UnrealNetwork.h"		// "NetCore" Build.cs에 추가
#include "Lobby/LobbyState.h"
 
ADSGameState::ADSGameState()
{
	bReplicates = true;			// 복제 허용
}
 
void ADSGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
 
	DOREPLIFETIME(ADSGameState, LobbyState);
}
 
void ADSGameState::BeginPlay()
{
	Super::BeginPlay();
 
	if (HasAuthority())		// 서버 일 때
	{
		CreateLobbyState();
		OnLobbyStateInitialized.Broadcast(LobbyState);
	}
}
 
void ADSGameState::CreateLobbyState()
{
	if (UWorld* World = GetWorld(); IsValid(World))
	{
		FActorSpawnParameters SpawnParams;
		LobbyState = World->SpawnActor<ALobbyState>(
			ALobbyState::StaticClass(),
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			SpawnParams
		);
		if (IsValid(LobbyState))
		{
			LobbyState->SetOwner(this);
		}
	}
}
 
void ADSGameState::OnRep_LobbyState()
{
	OnLobbyStateInitialized.Broadcast(LobbyState);
}