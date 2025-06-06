// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/UI/MatchResultPage.h"

#include "Character/TFPlayerController.h"
#include "Components/TextBlock.h"
#include "PlayerState/TFMatchPlayerState.h"

void UMatchResultPage::NativeConstruct()
{
	Super::NativeConstruct();

	LeftName = "";
	RightName = "";
	RateTime = 9;
	
	SetVisibility(ESlateVisibility::Hidden);  // 처음엔 숨겨진 상태
	

	// 초기화 시켜놓기
	ATFMatchPlayerState* PlayerState = GetPlayerState();
	if (IsValid(PlayerState))
	{
		PlayerState->OnMatchResultChanged.AddDynamic(this, &UMatchResultPage::UpdateResultData);
		OnMatchScoreChanged(PlayerState->MyMatchWins, PlayerState->OpponentMatchWins);
		SetMatchResultName();
	}
	else
	{
		ATFPlayerController* TFPlayerController = Cast<ATFPlayerController>(GetOwningPlayer());
		if (IsValid(TFPlayerController))
		{
			TFPlayerController->OnPlayerStateReplicated.AddUniqueDynamic(this, &UMatchResultPage::OnPlayerStateInitialized);
		}
	}
}

// 처음 세팅 해놓기 (이름, 상대방이름, 0, 0)
void UMatchResultPage::SetMatchResultName()
{
	// 내 이름
	if (LeftNameText) LeftNameText->SetText(FText::FromString(TEXT("나")));
	
	// 상대 이름
	if (RightNameText) RightNameText->SetText(FText::FromString(TEXT("상대방")));
}

void UMatchResultPage::OnMatchScoreChanged(int32 LeftScore, int32 RightScore)
{
	if (IsValid(LeftScoreText) && IsValid(RightScoreText))
	{
		LeftScoreText->SetText(FText::AsNumber(LeftScore));
		RightScoreText->SetText(FText::AsNumber(RightScore));
	}
}

void UMatchResultPage::HideResultPage()
{
	SetVisibility(ESlateVisibility::Hidden);
}

void UMatchResultPage::UpdateResultData(bool bIsWinner, int32 LeftScore, int32 RightScore)
{
	SetMatchResultName();
	
	FString ResultString = bIsWinner ? TEXT("VICTORY") : TEXT("DEFEAT");
	FLinearColor ResultColor = bIsWinner ? FLinearColor::Green : FLinearColor::Red;

	// 승리 패배 TEXT 출력
	if (ResultText)
	{
		ResultText->SetText(FText::FromString(ResultString));		// 승리 : Victory, 패배: Defeat
		ResultText->SetColorAndOpacity(FSlateColor(ResultColor));	// 승리 : 초록색, 패배 : 빨간색
	}

	// MatchScore 표시하기
	OnMatchScoreChanged(LeftScore, RightScore);

	// 위젯 표시
	SetVisibility(ESlateVisibility::Visible);

	// RateTime 초 후 자동으로 숨기기
	FTimerHandle HideTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(HideTimerHandle, this, &UMatchResultPage::HideResultPage, RateTime, false);
}


ATFMatchPlayerState* UMatchResultPage::GetPlayerState() const
{
	APlayerController* PlayerController = GetOwningPlayer();
	if (IsValid(PlayerController))
	{
		return PlayerController->GetPlayerState<ATFMatchPlayerState>();
	}
	return nullptr;
}

void UMatchResultPage::OnPlayerStateInitialized()
{
	// Get the PlayerState and bind to the score changed delegate
	ATFMatchPlayerState* PlayerState = GetPlayerState();
	if (IsValid(PlayerState))
	{
		PlayerState->OnMatchResultChanged.AddDynamic(this, &UMatchResultPage::UpdateResultData);
		OnMatchScoreChanged(PlayerState->MyMatchWins, PlayerState->OpponentMatchWins);
		SetMatchResultName();
	}

	// Unsubscribe from the OnPlayerStateChanged delegate
	ATFPlayerController* TFPlayerController = Cast<ATFPlayerController>(GetOwningPlayer());
	if (IsValid(TFPlayerController))
	{
		TFPlayerController->OnPlayerStateReplicated.RemoveDynamic(this, &UMatchResultPage::OnPlayerStateInitialized);
	}
}
