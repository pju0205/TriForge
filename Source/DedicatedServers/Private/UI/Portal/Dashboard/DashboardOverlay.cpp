// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Portal/Dashboard/DashboardOverlay.h"

#include "Components/Button.h"
#include "UI/Portal/Dashboard/Game/GamePage.h"
#include "UI/Portal/Dashboard/Career/CareerPage.h"
#include "UI/Portal/Dashboard/Leaderboard/LeaderboardPage.h"
#include "Components/WidgetSwitcher.h"
#include "UI/GameSessions/GameSessionsManager.h"
#include "UI/GameStats/GameStatsManager.h"

void UDashboardOverlay::NativeConstruct()
{
	Super::NativeConstruct();
	
	GameStatsManager = NewObject<UGameStatsManager>(this, GameStatsManagerClass);
	/*GameSessionsManager = NewObject<UGameSessionsManager>(this, GameSessionsManagerClass);*/

	
	// Game Page 관련
	/*GameSessionsManager->OnGameSessionCreated.AddDynamic(GamePage, &UGamePage::UpdateSessionListUI);*/
	/*GameSessionsManager->BroadcastGameSessionMessage.AddDynamic(GamePage, &UGamePage::SetStatusMessage);*/
	// Career Page 관련
	GameStatsManager->OnRetrieveMatchStatsResponseReceived.AddDynamic(CareerPage, &UCareerPage::OnRetrieveMatchStats);
	GameStatsManager->RetrieveMatchStatsStatusMesssage.AddDynamic(CareerPage, &UCareerPage::SetStatusMessage);
	// Leaderboard Page 관련
	GameStatsManager->OnRetrieveLeaderboard.AddDynamic(LeaderboardPage, &ULeaderboardPage::PopulateLeaderboard);
	GameStatsManager->RetrieveLeaderboardStatusMessage.AddDynamic(LeaderboardPage, &ULeaderboardPage::SetStatusMessage);
 
	Button_Game->OnClicked.AddDynamic(this, &UDashboardOverlay::ShowGamePage);
	Button_Career->OnClicked.AddDynamic(this, &UDashboardOverlay::ShowCareerPage);
	Button_Leaderboard->OnClicked.AddDynamic(this, &UDashboardOverlay::ShowLeaderboardPage);

	// 첫 화면 GamePage
	ShowGamePage();
}
 
void UDashboardOverlay::ShowGamePage()
{
	DisableButton(Button_Game);
	WidgetSwitcher->SetActiveWidget(GamePage);
}
 
void UDashboardOverlay::ShowCareerPage()
{
	DisableButton(Button_Career);
	WidgetSwitcher->SetActiveWidget(CareerPage);
	GameStatsManager->RetrieveMatchStats();
}
 
void UDashboardOverlay::ShowLeaderboardPage()
{
	DisableButton(Button_Leaderboard);
	WidgetSwitcher->SetActiveWidget(LeaderboardPage);
	GameStatsManager->RetrieveLeaderboard();
}

void UDashboardOverlay::DisableButton(UButton* Button) const
{
	Button_Game->SetIsEnabled(true);
	Button_Career->SetIsEnabled(true);
	Button_Leaderboard->SetIsEnabled(true);
	Button->SetIsEnabled(false);
}