// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Portal/Dashboard/Game/GamePage.h"

#include "UI/API/GameSessions/JoinGame.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "UI/API/GameSessions/HostGame.h"
#include "UI/GameSessions/GameSessionsManager.h"
#include "UI/Portal/Dashboard/Game/StatusMessage.h"

void UGamePage::NativeConstruct()
{
	Super::NativeConstruct();
 
	GameSessionsManager = NewObject<UGameSessionsManager>(this, GameSessionsManagerClass);
	GameSessionsManager->BroadcastGameSessionMessage.AddDynamic(this, &UGamePage::SetStatusMessage);
 
	JoinGameWidget->Button_JoinGame->OnClicked.AddDynamic(this, &UGamePage::JoinGameButtonClicked);
	HostGameWidget->Button_HostGame->OnClicked.AddDynamic(this, &UGamePage::HostGameButtonClicked);
}

void UGamePage::HostGameButtonClicked()
{
	ButtonSetIsEnabled(false);
	GameSessionsManager->HostGameSession();
}

void UGamePage::JoinGameButtonClicked()
{
	ButtonSetIsEnabled(false);
	GameSessionsManager->JoinGameSession();
}

void UGamePage::ButtonSetIsEnabled(bool bClicked)
{
	if (bClicked)
	{
		HostGameWidget->Button_HostGame->SetIsEnabled(bClicked);
		JoinGameWidget->Button_JoinGame->SetIsEnabled(bClicked);
	}
	else
	{
		HostGameWidget->Button_HostGame->SetIsEnabled(bClicked);
		JoinGameWidget->Button_JoinGame->SetIsEnabled(bClicked);
	}
}


void UGamePage::SetStatusMessage(const FString& Message, bool bShouldResetWidgets)
{
	StatusMessageWidget->TextBlock_StatusMessage->SetText(FText::FromString(Message));
	ButtonSetIsEnabled(bShouldResetWidgets);
}