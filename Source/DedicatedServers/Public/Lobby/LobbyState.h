// Fill out your copyright notice in the Description page of Project Settings.
 
#pragma once
 
#include "CoreMinimal.h"
#include "LobbyPlayerInfo.h"
#include "GameFramework/Info.h"
#include "LobbyState.generated.h"
 
USTRUCT()
struct FLobbyPlayerInfoDelta
{
	GENERATED_BODY()
 
	UPROPERTY()
	TArray<FLobbyPlayerInfo> AddedPlayers{};
 
	UPROPERTY()
	TArray<FLobbyPlayerInfo> RemovedPlayers{};

	UPROPERTY()
	TArray<FLobbyPlayerInfo> UpdatedPlayers{};
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerInfoChanged, const FLobbyPlayerInfo&, PlayerInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerCountChanged, int32, PlayerCount);

// AInfo 는 AGameMode의 부모클래스
// AInfo 기반 클래스는 서버에서 하나만 존재하며, 모든 클라이언트에게 복제됨
UCLASS()
class DEDICATEDSERVERS_API ALobbyState : public AInfo
{
	GENERATED_BODY()
 
public:
	virtual void BeginPlay() override;
	
	ALobbyState();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// playerInfo 관련
	UPROPERTY(BlueprintAssignable)
	FOnPlayerInfoChanged OnPlayerInfoAdded;
 
	UPROPERTY(BlueprintAssignable)
	FOnPlayerInfoChanged OnPlayerInfoRemoved;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerInfoChanged OnPlayerInfoUpdated;

	// PlayerCount 관련
	UPROPERTY(BlueprintAssignable)
	FOnPlayerCountChanged OnPlayerCountChanged;

	// UserInfo 관련
	void AddPlayerInfo(const FLobbyPlayerInfo& PlayerInfo);
	void RemovePlayerInfo(const FString& Username);
	TArray<FLobbyPlayerInfo> GetPlayers() const;
	
	// Ready 관련
	void ResetAllReadyStates();
	void SetPlayerReadyState(const FString& Username, bool bIsReady);
	bool AreAllPlayersReady();
	
protected:
 
	UFUNCTION()
	void OnRep_LobbyPlayerInfo();
 	
private:
 
	UPROPERTY(ReplicatedUsing=OnRep_LobbyPlayerInfo)
	FLobbyPlayerInfoArray PlayerInfoArray;

	UPROPERTY()
	FLobbyPlayerInfoArray LastPlayerInfoArray;
 
	FLobbyPlayerInfoDelta ComputePlayerInfoDelta(const TArray<FLobbyPlayerInfo>& OldArray, const TArray<FLobbyPlayerInfo>& NewArray);
};