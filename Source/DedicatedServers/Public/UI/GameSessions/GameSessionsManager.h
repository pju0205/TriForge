// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "UI/HTTP/HTTPRequestManager.h"
#include "GameSessionsManager.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBroadcastGameSessionMessage, const FString&, StatusMessage, bool, bShouldResetJoinGameButton);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameSessionCreated, const FDSGameSession&, GameSession);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRetrieveGameSession, const FDSGameSession&, GameSession);

/**
 * 
 */
UCLASS(Blueprintable)
class DEDICATEDSERVERS_API UGameSessionsManager : public UHTTPRequestManager
{
	GENERATED_BODY()
public:
	// Session Message 델리게이트
	UPROPERTY(BlueprintAssignable)
	FAPIStatusMessage BroadcastGameSessionMessage;

	// GameSessionCreated 델리게이트
	UPROPERTY(BlueprintAssignable)
	FOnGameSessionCreated OnGameSessionCreated;

	UPROPERTY(BlueprintAssignable)
	FOnRetrieveGameSession OnRetrieveGameSession;
 
	void QuickMatchGameSession();
	void HostGameSession();
	void RetrieveGameSessions();

	// 서버에 접속 시도하는 중요한 함수
	void TryCreatePlayerSession(const FString& PlayerId, const FString& GameSessionId);
private:
	
	void FindOrCreateGameSession_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void RetrieveGameSession_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void HostGameSession_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void CreatePlayerSession_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	FString GetUniquePlayerId() const;
	void HandleGameSessionStatus(const FString& Status, const FString& SessionId);
	
 
	FTimerHandle CreateSessionTimer;
};
