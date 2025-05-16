
#include "Lobby/LobbyPlayerInfo.h"

#include "Lobby/LobbyState.h"


void FLobbyPlayerInfoArray::AddPlayer(const FLobbyPlayerInfo& NewPlayerInfo)
{
	int32 Index = Players.Add(NewPlayerInfo);
	MarkItemDirty(Players[Index]);			// FastArraySerializer 시스템에 변경된 부분 복제하라고 알림. 클라이언트에게 복제를 트리거함.
	Players[Index].PostReplicatedAdd(*this);	// 클라이언트 쪽에서만 호출되는 콜백
}
 
void FLobbyPlayerInfoArray::RemovePlayer(const FString& Username)
{
	// 루프를 돌면서 Username과 일치하는 플레이어를 찾음
	for (int32 PlayerIndex = 0; PlayerIndex < Players.Num(); ++PlayerIndex)
	{
		FLobbyPlayerInfo& PlayerInfo = Players[PlayerIndex];
		if (PlayerInfo.Username == Username)
		{
			PlayerInfo.PreReplicatedRemove(*this);	// 복제되기 전에 삭제될 것을 알리는 콜백
			Players.RemoveAtSwap(PlayerIndex);		// 해당 플레이어를 빠르게 삭제 (성능상 일반 RemoveAt보다 더 빠름)
			MarkArrayDirty();						// 네트워크 복제 시스템에 전체 배열이 변경됨을 통지
			break;
		}
	}
}

// Ready 상태 변경시키는 함수
void FLobbyPlayerInfoArray::SetPlayerReadyState(const FString& Username, bool bIsReady)
{
	for (int32 i = 0; i < Players.Num(); ++i)
	{
		if (Players[i].Username == Username)
		{
			if (Players[i].ReadyState != bIsReady)
			{
				UE_LOG(LogTemp, Warning, TEXT("[서버] ReadyState 변경: %s -> %s"), *Username, bIsReady ? TEXT("true") : TEXT("false"));
				Players[i].ReadyState = bIsReady;
				MarkItemDirty(Players[i]);
				
				if (OwnerState)
				{
					OwnerState->OnPlayerInfoUpdated.Broadcast(Players[i]);
					// LobbyState에 해당 플레이어 PalyerInfo가 Updated 됐다고 전부 알리기
					// LobbyPlayerBox에서 자동으로 호출되면서 State 업데이트함
				}
			}
			return;
		}
	}
}

// 전부 Ready 됐는지 확인
bool FLobbyPlayerInfoArray::AreAllPlayersReady() const
{
	for (const FLobbyPlayerInfo& Info : Players)
	{
		if (!Info.ReadyState)
		{
			return false;
		}
	}
	return true;
}
