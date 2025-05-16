// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Lobby/PlayerReadyButton.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Player/DSPlayerController.h"
#include "UI/Lobby/PlayerLabel.h"


void UPlayerReadyButton::NativeConstruct()
{
	Super::NativeConstruct();

	bLocalIsReady = false;

	if (Button_Ready)
	{
		Button_Ready->OnClicked.AddDynamic(this, &UPlayerReadyButton::OnReadyButtonClicked);
	}
}

void UPlayerReadyButton::OnReadyButtonClicked()
{
	// 상태 반전
	bLocalIsReady = !bLocalIsReady;

	// 서버에 알림
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (ADSPlayerController* DSPC = Cast<ADSPlayerController>(PC))
		{
			// 이제 bLocalIsReady를 서버에 알림
			DSPC->Server_SetReadyState(bLocalIsReady);
		}
	}

	// UI 업데이트
	if (TextBlock_Ready)
	{
		TextBlock_Ready->SetText(!bLocalIsReady ? FText::FromString(TEXT("Ready")) : FText::FromString(TEXT("Cancel")));
	}
}


