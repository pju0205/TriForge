
#include "Lobby/LobbyPlayerInfo.h"


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