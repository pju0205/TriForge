// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DSLocalPlayerSubsystem.h"

#include "UI/Portal/Interfaces/PortalManagement.h"


// 로그인 했을 때 생성되는 토큰 저장소
void UDSLocalPlayerSubsystem::InitializeTokens(const FDSAuthenticationResult& AuthResult, TScriptInterface<IPortalManagement> PortalManagement)
{
	// Portal Manager에서 받아온 토큰
	AuthenticationResult = AuthResult;
	PortalManagementInterface = PortalManagement;
	SetRefreshTokenTimer();
}
 
void UDSLocalPlayerSubsystem::SetRefreshTokenTimer()
{
	UWorld* World = GetWorld();
	if (IsValid(World) && IsValid(PortalManagementInterface.GetObject()))
	{
		FTimerDelegate RefreshDelegate;
		RefreshDelegate.BindLambda([this](){  PortalManagementInterface->RefreshTokens(AuthenticationResult.RefreshToken); });
		World->GetTimerManager().SetTimer(RefreshTimer, RefreshDelegate, TokenRefreshInterval, false);
	}
}

void UDSLocalPlayerSubsystem::UpdateTokens(const FString& AccessToken, const FString& IdToken)
{
	AuthenticationResult.AccessToken = AccessToken;
	AuthenticationResult.IdToken = IdToken;
	AuthenticationResult.Dump();
	SetRefreshTokenTimer();
}


FDSAuthenticationResult UDSLocalPlayerSubsystem::GetAuthResult() const
{
	return AuthenticationResult;
}