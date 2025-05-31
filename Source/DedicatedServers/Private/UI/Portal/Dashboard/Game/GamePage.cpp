// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Portal/Dashboard/Game/GamePage.h"

#include "UI/API/GameSessions/QuickMatchGame.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Game/DSGameInstanceSubsystem.h"
#include "UI/API/GameSessions/HostGame.h"
#include "UI/API/GameSessions/JoinGame.h"
#include "UI/GameSessions/GameSessionsManager.h"
#include "UI/HTTP/HTTPRequestTypes.h"
#include "Player/DSLocalPlayerSubsystem.h"
#include "UI/Portal/Dashboard/Game/GameListCard.h"
#include "UI/Portal/Dashboard/Game/StatusMessage.h"

void UGamePage::NativeConstruct()
{
	Super::NativeConstruct();
 
	GameSessionsManager = NewObject<UGameSessionsManager>(this, GameSessionsManagerClass);
	GameSessionsManager->BroadcastGameSessionMessage.AddDynamic(this, &UGamePage::SetStatusMessage);
	GameSessionsManager->OnRetrieveGameSession.AddDynamic(this, &UGamePage::UpdateSessionListUI);			// 추가

	// 버튼 클릭 바인딩
	QuickMatchGameWidget->Button_QuickMatchGame->OnClicked.AddDynamic(this, &UGamePage::QuickMatchGameButtonClicked);
	HostGameWidget->Button_HostGame->OnClicked.AddDynamic(this, &UGamePage::HostGameButtonClicked);
	JoinGameWidget->Button_JoinGame->OnClicked.AddDynamic(this, &UGamePage::JoinGameButtonClicked);
	RefreshButtonWidget->OnClicked.AddDynamic(this, &UGamePage::RefreshButtonClicked);

	RefreshButtonClicked();
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

	if (!bHasSelectedSession)
	{
		SetStatusMessage(TEXT("Please select a session."), true);
		return;
	}

	
	if (UDSLocalPlayerSubsystem* DSLocalPlayerSubsystem = GameSessionsManager->GetDSLocalPlayerSubsystem(); IsValid(DSLocalPlayerSubsystem))
	{
		FString Username = DSLocalPlayerSubsystem->Username; // 또는 GetPlayerId() 등
		FString SessionId = SelectedGameSessionID;

		GameSessionsManager->TryCreatePlayerSession(Username, SessionId);
	}
	else
	{
		SetStatusMessage(TEXT("Player session limit reached"), true);
	}
}

void UGamePage::RefreshButtonClicked()			// 추가
{
	ButtonSetIsEnabled(false);
	ScrollBox_GameList->ClearChildren(); // 목록 초기화 하고
	
	GameSessionsManager->RetrieveGameSessions();	 // 다시 리스트 요청
}

void UGamePage::ButtonSetIsEnabled(bool bClicked)
{
	if (bClicked)
	{
		HostGameWidget->Button_HostGame->SetIsEnabled(bClicked);
		QuickMatchGameWidget->Button_QuickMatchGame->SetIsEnabled(bClicked);
		JoinGameWidget->Button_JoinGame->SetIsEnabled(bClicked);
		RefreshButtonWidget->SetIsEnabled(bClicked);
	}
	else
	{
		HostGameWidget->Button_HostGame->SetIsEnabled(bClicked);
		QuickMatchGameWidget->Button_QuickMatchGame->SetIsEnabled(bClicked);
		JoinGameWidget->Button_JoinGame->SetIsEnabled(bClicked);
		RefreshButtonWidget->SetIsEnabled(bClicked);
	}
}


// Card Widget 생성용
void UGamePage::UpdateSessionListUI(const FDSGameSession& GameSession)
{
	if (!GameListCardClass || !ScrollBox_GameList) return;

	// 새로운 카드 위젯 생성
	UGameListCard* GameListCard = CreateWidget<UGameListCard>(this, GameListCardClass);
	if (!GameListCard) return;

	// 카드에 세션 정보 입력
	const FString SessionName = GameSession.CreatorId;
	const int32 PlayerCount = GameSession.CurrentPlayerSessionCount;
	const int32 MaxPlayerCount = GameSession.MaximumPlayerSessionCount;

	// 방 안에 아무도 없으면 추가 안하기
	if (PlayerCount == 0) return;
	
	GameListCard->SetSessionInfo(SessionName, PlayerCount, MaxPlayerCount);

	// GameSessionId 저장
	GameListCard->GameSessionId = GameSession.GameSessionId;
	GameListCard->OnCardSelected.AddDynamic(this, &UGamePage::HandleCardSelected);

	// ScrollBox에 추가
	ScrollBox_GameList->AddChild(GameListCard);
}

void UGamePage::SetStatusMessage(const FString& Message, bool bShouldResetWidgets)
{
	StatusMessageWidget->TextBlock_StatusMessage->SetText(FText::FromString(Message));
	ButtonSetIsEnabled(bShouldResetWidgets);
}


void UGamePage::HandleCardSelected(const FString& GameSessionID)
{
	// 같은 카드 눌렀을 경우 선택 해제
	if (SelectedCard && SelectedCard->GameSessionId == GameSessionID)
	{
		SelectedCard->SetSelected(false);
		SelectedCard = nullptr;
		SelectedGameSessionID = TEXT("");
		bHasSelectedSession = false;
		return;
	}

	// 이전 선택된 카드 비활성화
	if (SelectedCard)
	{
		SelectedCard->SetSelected(false);
	}

	// 새로운 카드 선택
	for (UWidget* Widget : ScrollBox_GameList->GetAllChildren())
	{
		if (UGameListCard* Card = Cast<UGameListCard>(Widget))
		{
			if (Card->GameSessionId == GameSessionID)
			{
				SelectedCard = Card;
				SelectedCard->SetSelected(true);
				SelectedGameSessionID = GameSessionID;
				bHasSelectedSession = true;
				// 이 상태로 선택된 채로 JoinGame 버튼을 누르면 실행됨
				break;
			}
		}
	}
}
