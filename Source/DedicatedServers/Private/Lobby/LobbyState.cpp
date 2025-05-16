// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/LobbyState.h"

#include "Net/UnrealNetwork.h"		// NetCore 사용


void ALobbyState::BeginPlay()
{
	Super::BeginPlay();

	PlayerInfoArray.OwnerState = this;
}

ALobbyState::ALobbyState()
{
	PrimaryActorTick.bCanEverTick = false;		// Tick이 필요 없다는 뜻
	bReplicates = true;							// 네트워크를 통해 이 Actor를 복제함
	bAlwaysRelevant = true;						// 거리에 관계없이 항상 클라이언트에 복제됨
}

// 멀티 처리
void ALobbyState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ALobbyState, PlayerInfoArray);
}

// 로비 입장시 정보 추가
void ALobbyState::AddPlayerInfo(const FLobbyPlayerInfo& PlayerInfo)
{
	PlayerInfoArray.AddPlayer(PlayerInfo);
}

// 로비 퇴장시 정보 삭제
void ALobbyState::RemovePlayerInfo(const FString& Username)
{
	PlayerInfoArray.RemovePlayer(Username);
}

// 로비 Ready 상태 변경 함수
void ALobbyState::SetPlayerReadyState(const FString& Username, bool bIsReady)
{
	PlayerInfoArray.SetPlayerReadyState(Username, bIsReady);
}

// 로비상 모든 플레이어가 Ready 했는지 확인하는 함수
bool ALobbyState::AreAllPlayersReady()
{
	return PlayerInfoArray.AreAllPlayersReady();
}

// player 정보 Get 함수
TArray<FLobbyPlayerInfo> ALobbyState::GetPlayers() const
{
	return PlayerInfoArray.Players;
}


// 멀티처리 실행 함수
void ALobbyState::OnRep_LobbyPlayerInfo()
{
	FLobbyPlayerInfoDelta Delta = ComputePlayerInfoDelta(LastPlayerInfoArray.Players, PlayerInfoArray.Players);
	for (const auto& PlayerInfo : Delta.AddedPlayers)
	{
		OnPlayerInfoAdded.Broadcast(PlayerInfo);
	}
	for (const auto& PlayerInfo : Delta.RemovedPlayers)
	{
		OnPlayerInfoRemoved.Broadcast(PlayerInfo);
	}
	for (const auto& PlayerInfo : Delta.UpdatedPlayers)
	{
		OnPlayerInfoUpdated.Broadcast(PlayerInfo);
	}
 
	LastPlayerInfoArray = PlayerInfoArray;
}

// 사용자 지정 구조체 정의 함수 적용
FLobbyPlayerInfoDelta ALobbyState::ComputePlayerInfoDelta(const TArray<FLobbyPlayerInfo>& OldArray, const TArray<FLobbyPlayerInfo>& NewArray)
{
	FLobbyPlayerInfoDelta Delta;	// 헤더 파일에 정의한 구조체

	// Username을 키로 하여 TMap 생성
	// 빠른 Contains() 연산을 가능하게 만듦 (O(1))
	TMap<FString, const FLobbyPlayerInfo*> OldMap;
	TMap<FString, const FLobbyPlayerInfo*> NewMap;

	
	for (const auto& PlayerInfo : OldArray)
	{
		OldMap.Add(PlayerInfo.Username, &PlayerInfo);
	}
	
	for (const auto& PlayerInfo : NewArray)
	{
		NewMap.Add(PlayerInfo.Username, &PlayerInfo);
	}
	
	// OldArray에는 있는데 NewArray에는 없는 경우 → 제거된 플레이어
	for (const auto& OldPlayerInfo : OldArray)
	{
		if (!NewMap.Contains(OldPlayerInfo.Username))
		{
			Delta.RemovedPlayers.Add(OldPlayerInfo);
		}
	}
	// NewArray에는 있는데 OldArray에는 없는 경우 → 새로 추가된 플레이어
	for (const auto& NewPlayerInfo : NewArray)
	{
		// 기존에 없는 username이면 새로 추가
		if (!OldMap.Contains(NewPlayerInfo.Username))
		{
			Delta.AddedPlayers.Add(NewPlayerInfo);
		}
		else
		{
			// 기존에 있는 Username이면 옛날 정보와 비교해서 값을 새로운 값으로 업데이트
			const FLobbyPlayerInfo* OldPlayerInfo = OldMap[NewPlayerInfo.Username];
			if (OldPlayerInfo && *OldPlayerInfo != NewPlayerInfo)		// 값이 바뀐 경우
			{
				Delta.UpdatedPlayers.Add(NewPlayerInfo);
			}
		}
	}

	/*// NewArray에는 있는데 OldArray에는 없는 경우 → 새로 추가된 플레이어
	for (const auto& NewPlayerInfo : NewArray)
	{
		if (!OldMap.Contains(NewPlayerInfo.Username))
		{
			Delta.AddedPlayers.Add(NewPlayerInfo);
		}
	}*/
 
	return Delta;	// 해당 정보 구조체로 반환
}