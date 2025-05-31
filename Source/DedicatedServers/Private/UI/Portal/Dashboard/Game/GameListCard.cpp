// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Portal/Dashboard/Game/GameListCard.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UGameListCard::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Button_Session)
	{
		Button_Session->OnClicked.AddDynamic(this, &UGameListCard::HandleClick);
	}
}

// GameList Button 클릭하면 델리게이트 실행 -> GamePage로
void UGameListCard::HandleClick()
{
	OnCardSelected.Broadcast(GameSessionId);
}

void UGameListCard::SetSessionInfo(const FString& SessionName, int32 Player, int32 MaxPlayer) const
{
	TextBlock_SessionName->SetText(FText::FromString(SessionName + TEXT("'s Room")));
	TextBlock_Players->SetText(FText::FromString(FString::FromInt(Player) + TEXT("/") + FString::FromInt(MaxPlayer)));

	// 플레이어 수가 가득 찼으면 버튼 비활성화
	if (Player >= MaxPlayer)
	{
		if (IsValid(Button_Session))
		{
			Button_Session->SetIsEnabled(false);
		}
	}
	else
	{
		if (IsValid(Button_Session))
		{
			Button_Session->SetIsEnabled(true);
		}
	}
}

void UGameListCard::SetSelected(bool bSelected)
{
	bIsSelected = bSelected;

	// 시각적 처리 (배경 색상 변경)
	if (bIsSelected)
	{
		// 예시: 선택되었을 때 배경색 변경
		SetColorAndOpacity(FLinearColor(0.2f, 0.5f, 1.0f, 1.0f));
	}
	else
	{
		SetColorAndOpacity(FLinearColor::White);
	}
}