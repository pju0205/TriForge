﻿// Fill out your copyright notice in the Description page of Project Settings.
// 사용자 계정당 정보 저장 용도 (토큰 관련)


#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "UI/HTTP/HTTPRequestTypes.h"
#include "DSLocalPlayerSubsystem.generated.h"


class IPortalManagement;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UDSLocalPlayerSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
	
public:
	void InitializeTokens(const FDSAuthenticationResult& AuthResult, TScriptInterface<IPortalManagement> PortalManagement);
	void SetRefreshTokenTimer();
	void UpdateTokens(const FString& AccessToken, const FString& IdToken);
	FDSAuthenticationResult GetAuthResult() const;

	FString Username;
	FString Email;
	
private:
	UPROPERTY()
	FDSAuthenticationResult AuthenticationResult;
 
	UPROPERTY()
	TScriptInterface<IPortalManagement> PortalManagementInterface;

	// 75% of an hour (the expiration time for AccessToken and IdToken)
	float TokenRefreshInterval = 2700.f;	// 토큰 재성성 시간
	FTimerHandle RefreshTimer;
};
