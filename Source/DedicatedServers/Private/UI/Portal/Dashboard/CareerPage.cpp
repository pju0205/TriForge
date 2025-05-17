// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Portal/Dashboard/CareerPage.h"

#include "Components/TextBlock.h"
#include "UI/HTTP/HTTPRequestTypes.h"

void UCareerPage::NativeConstruct()
{
	Super::NativeConstruct();

	// 항목 초기화 시켜놓기
	TextBlock_Username->SetText(FText::GetEmpty());
	TextBlock_Wins->SetText(FText::AsNumber(0));
	TextBlock_Losses->SetText(FText::AsNumber(0));
}

// Stats이 결정나면 변경하기
void UCareerPage::OnRetrieveMatchStats(const FDSRetrieveMatchStatsResponse& RetrieveMatchStatsResponse)
{
	TextBlock_Username->SetText(FText::FromString(RetrieveMatchStatsResponse.username));
	TextBlock_Wins->SetText(FText::AsNumber(RetrieveMatchStatsResponse.matchWins));
	TextBlock_Losses->SetText(FText::AsNumber(RetrieveMatchStatsResponse.matchLosses));
}

void UCareerPage::SetStatusMessage(const FString& StatusMessage, bool bShouldResetWidgets)
{
	TextBlock_StatusMessage->SetText(FText::FromString(StatusMessage));
}