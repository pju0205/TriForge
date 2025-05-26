// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Portal/Dashboard/Game/GamePage.h"

#include "UI/API/GameSessions/QuickMatchGame.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Game/DSGameInstanceSubsystem.h"
#include "UI/API/GameSessions/HostGame.h"
#include "UI/API/GameSessions/JoinGame.h"
#include "UI/GameSessions/GameSessionsManager.h"
#include "UI/Portal/Dashboard/Game/StatusMessage.h"

void UGamePage::NativeConstruct()
{
	Super::NativeConstruct();
 
	GameSessionsManager = NewObject<UGameSessionsManager>(this, GameSessionsManagerClass);
	GameSessionsManager->BroadcastGameSessionMessage.AddDynamic(this, &UGamePage::SetStatusMessage);
 
	QuickMatchGameWidget->Button_QuickMatchGame->OnClicked.AddDynamic(this, &UGamePage::QuickMatchGameButtonClicked);
	HostGameWidget->Button_HostGame->OnClicked.AddDynamic(this, &UGamePage::HostGameButtonClicked);
	JoinGameWidget->Button_JoinGame->OnClicked.AddDynamic(this, &UGamePage::JoinGameButtonClicked);

	/*if (UGameInstance* GI = GetGameInstance())
	{
		if (UDSGameInstanceSubsystem* SessionSubsystem = GI->GetSubsystem<UDSGameInstanceSubsystem>())
		{
			SessionSubsystem->OnGameSessionsUpdated.AddDynamic(this, &UGamePage::UpdateSessionListUI);

			// 초기 갱신도 요청
			SessionSubsystem->UpdateSessionsFromServer();
		}
	}*/
}

void UGamePage::QuickMatchGameButtonClicked()
{
	ButtonSetIsEnabled(false);
	GameSessionsManager->QuickMatchGameSession();
}

void UGamePage::HostGameButtonClicked()
{
	ButtonSetIsEnabled(false);
	GameSessionsManager->HostGameSession();
}

void UGamePage::JoinGameButtonClicked()
{
	ButtonSetIsEnabled(false);
	// 만들기
}

void UGamePage::ButtonSetIsEnabled(bool bClicked)
{
	if (bClicked)
	{
		HostGameWidget->Button_HostGame->SetIsEnabled(bClicked);
		QuickMatchGameWidget->Button_QuickMatchGame->SetIsEnabled(bClicked);
		JoinGameWidget->Button_JoinGame->SetIsEnabled(bClicked);
	}
	else
	{
		HostGameWidget->Button_HostGame->SetIsEnabled(bClicked);
		QuickMatchGameWidget->Button_QuickMatchGame->SetIsEnabled(bClicked);
		JoinGameWidget->Button_JoinGame->SetIsEnabled(bClicked);
	}
}


void UGamePage::SetStatusMessage(const FString& Message, bool bShouldResetWidgets)
{
	StatusMessageWidget->TextBlock_StatusMessage->SetText(FText::FromString(Message));
	ButtonSetIsEnabled(bShouldResetWidgets);
}

/*void UGamePage::UpdateSessionListUI()
{
	if (!GameListCardClass || !ScrollBox_GameList) return;

	// 기존 아이템 제거
	ScrollBox_GameList->ClearChildren();
	
	
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UDSGameInstanceSubsystem* SessionSubsystem = GI->GetSubsystem<UDSGameInstanceSubsystem>())
		{
			// SessionSubsystem->CachedGameSessions 사용해서 ListView 갱신
			for (const auto& Pair : SessionSubsystem->CachedGameSessions)
			{
				const FDSGameSession& Session = Pair.Value;

				// 카드 위젯 생성
				UGameListCard* Card = CreateWidget<UGameListCard>(this, GameListCardClass);
				if (!Card) continue;

				// 세션 정보 전달
				Card->SetSessionInfo(Session.Name, Session.CurrentPlayerSessionCount, Session.MaximumPlayerSessionCount);

				// 패널에 추가
				ScrollBox_GameList->AddChild(Card);
			}
		}
	}
}*/