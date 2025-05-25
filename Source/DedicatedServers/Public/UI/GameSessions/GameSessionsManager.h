// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "UI/HTTP/HTTPRequestManager.h"
#include "GameSessionsManager.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBroadcastGameSessionMessage, const FString&, StatusMessage, bool, bShouldResetJoinGameButton);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameSessionCreated, const FDSGameSession&, GameSession);

/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UGameSessionsManager : public UHTTPRequestManager
{
	GENERATED_BODY()


public:
	// Session Message 델리게이트
	UPROPERTY(BlueprintAssignable)
	FAPIStatusMessage BroadcastGameSessionMessage;

	// GameSessionCreated 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "GameSession")
	FOnGameSessionCreated OnGameSessionCreated;
 
	void JoinGameSession();
	void HostGameSession();
private:
 
	void FindOrCreateGameSession_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void HostGameSession_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void CreatePlayerSession_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	FString GetUniquePlayerId() const;
	void HandleGameSessionStatus(const FString& Status, const FString& SessionId);
	void TryCreatePlayerSession(const FString& PlayerId, const FString& GameSessionId);
 
	FTimerHandle CreateSessionTimer;
};
