#pragma once
 
#include "Net/Serialization/FastArraySerializer.h"
 
#include "LobbyPlayerInfo.generated.h"


// 자체적으로 만든 파일 Build.cs 에서 NetCore 추가
// shift shift 해서 FastArraySerializer 검색해서 헤더 파일 확인하기

// 개별 항목 Struct
// FFastArraySerializerItem 상속 : 상속해야 변화 추적 대상이 될 수 있음
USTRUCT(BlueprintType)
struct FLobbyPlayerInfo : public FFastArraySerializerItem
{
	GENERATED_BODY()
 
	FLobbyPlayerInfo() {}
	FLobbyPlayerInfo(const FString& Name) : Username(Name) {}
 
	UPROPERTY(BlueprintReadWrite)
	FString Username{};
};


// 전체 목록 Struct
// FFastArraySerializer 상속: 이 구조체가 NetDeltaSerialize를 통해 변화된 항목만 복제 가능하게 만들어줌
USTRUCT()
struct FLobbyPlayerInfoArray : public FFastArraySerializer
{
	GENERATED_BODY()
 
	UPROPERTY()
	TArray<FLobbyPlayerInfo> Players;

	// 복제 과정에서 변경된 부분만 네트워크 전송되도록 하는 핵심 함수
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FastArrayDeltaSerialize<FLobbyPlayerInfo, FLobbyPlayerInfoArray>(Players, DeltaParams, *this);
	}
 
	void AddPlayer(const FLobbyPlayerInfo& NewPlayerInfo);
	void RemovePlayer(const FString& Username);
};


// NetDeltaSerialize를 쓸 거라고 명시
template<>
struct TStructOpsTypeTraits<FLobbyPlayerInfoArray> : public TStructOpsTypeTraitsBase2<FLobbyPlayerInfoArray>
{
	enum
	{
		// 없으면 변경 추적 기능이 동작하지 않음
		WithNetDeltaSerializer = true,
	};
};