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
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerInfoChanged, const FLobbyPlayerInfo&, PlayerInfo);

// AInfo 는 AGameMode의 부모클래스
// AInfo 기반 클래스는 서버에서 하나만 존재하며, 모든 클라이언트에게 복제됨
UCLASS()
class DEDICATEDSERVERS_API ALobbyState : public AInfo
{
	GENERATED_BODY()
 
public:
	ALobbyState();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
 
	UPROPERTY(BlueprintAssignable)
	FOnPlayerInfoChanged OnPlayerInfoAdded;
 
	UPROPERTY(BlueprintAssignable)
	FOnPlayerInfoChanged OnPlayerInfoRemoved;
 
	void AddPlayerInfo(const FLobbyPlayerInfo& PlayerInfo);
	void RemovePlayerInfo(const FString& Username);
	TArray<FLobbyPlayerInfo> GetPlayers() const;
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