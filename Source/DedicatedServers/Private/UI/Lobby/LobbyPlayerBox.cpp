// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Lobby/LobbyPlayerBox.h"

#include "Components/ScrollBox.h"
#include "Game/DSGameState.h"
#include "Lobby/LobbyState.h"
#include "Lobby/LobbyPlayerInfo.h"
#include "UI/Lobby/PlayerLabel.h"

void ULobbyPlayerBox::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ADSGameState* DSGameState = GetWorld()->GetGameState<ADSGameState>();
	if (!IsValid(DSGameState)) return;

	if (IsValid(DSGameState->LobbyState))
	{
		OnLobbyStateInitialized( DSGameState->LobbyState);

		// 이미 LobbyState가 살아있는 경우, 직접 갱신 강제
		/*UpdatePlayerInfo(DSGameState->LobbyState);*/
	}
	else
	{
		DSGameState->OnLobbyStateInitialized.AddDynamic(this, &ULobbyPlayerBox::OnLobbyStateInitialized);
	}
}

// LobbyState 초기화 함수
void ULobbyPlayerBox::OnLobbyStateInitialized(ALobbyState* LobbyState)
{
	if (!IsValid(LobbyState)) return;

	// 바인딩 중복 방지
	LobbyState->OnPlayerInfoAdded.RemoveDynamic(this, &ULobbyPlayerBox::CreateAndAddPlayerLabel);
	LobbyState->OnPlayerInfoRemoved.RemoveDynamic(this, &ULobbyPlayerBox::OnPlayerRemoved);
	LobbyState->OnPlayerInfoUpdated.RemoveDynamic(this, &ULobbyPlayerBox::OnPlayerInfoStateUpdated);

	// 바인딩 재설정
	LobbyState->OnPlayerInfoAdded.AddDynamic(this, &ULobbyPlayerBox::CreateAndAddPlayerLabel);
	LobbyState->OnPlayerInfoRemoved.AddDynamic(this, &ULobbyPlayerBox::OnPlayerRemoved);
	LobbyState->OnPlayerInfoUpdated.AddDynamic(this, &ULobbyPlayerBox::OnPlayerInfoStateUpdated);
	
	UpdatePlayerInfo(LobbyState); // 1. 최초 1회 바로 실행
}

// PlayerInfo update시 실행 함수
void ULobbyPlayerBox::UpdatePlayerInfo(ALobbyState* LobbyState)
{
	ScrollBox_PlayerInfo->ClearChildren();
	for (const FLobbyPlayerInfo& PlayerInfo : LobbyState->GetPlayers())
	{
		CreateAndAddPlayerLabel(PlayerInfo);	// 2. 만들거나 업데이트시키기 (최초시 만듦)
	}
}

// 초기화, 업데이트시 실행되는 함수
void ULobbyPlayerBox::CreateAndAddPlayerLabel(const FLobbyPlayerInfo& PlayerInfo)
{
	if (FindPlayerLabel(PlayerInfo.Username)) return;
	
	UPlayerLabel* PlayerLabel = CreateWidget<UPlayerLabel>(this, PlayerLabelClass);
	if (!IsValid(PlayerLabel)) return;
	
	PlayerLabel->SetUsername(PlayerInfo.Username);			// UserName 부착
	PlayerLabel->SetReadyState(PlayerInfo.ReadyState);		// UserReadyState 부착
	
	ScrollBox_PlayerInfo->AddChild(PlayerLabel);			// 3. 최초일 때 name, ReadtState 부착시킴
}

void ULobbyPlayerBox::OnPlayerRemoved(const FLobbyPlayerInfo& PlayerInfo)
{
	if (UPlayerLabel* PlayerLabel = FindPlayerLabel(PlayerInfo.Username))
	{
		ScrollBox_PlayerInfo->RemoveChild(PlayerLabel);
	}
}

void ULobbyPlayerBox::OnPlayerInfoStateUpdated(const FLobbyPlayerInfo& PlayerInfo)
{
	if (UPlayerLabel* PlayerLabel = FindPlayerLabel(PlayerInfo.Username))
	{
		PlayerLabel->SetReadyState(PlayerInfo.ReadyState);
	}
}

UPlayerLabel* ULobbyPlayerBox::FindPlayerLabel(const FString& Username)
{
	for (UWidget* Child : ScrollBox_PlayerInfo->GetAllChildren())
	{
		UPlayerLabel* PlayerLabel = Cast<UPlayerLabel>(Child);
		if (IsValid(PlayerLabel) && PlayerLabel->GetUsername() == Username)
		{
			return PlayerLabel;
		}
	}
	return nullptr;
}
