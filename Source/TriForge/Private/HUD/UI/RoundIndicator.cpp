// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/UI/RoundIndicator.h"

#include "Character/TFPlayerController.h"
#include "Components/Image.h"
#include "PlayerState/TFMatchPlayerState.h"

void URoundIndicator::NativeConstruct()
{
	Super::NativeConstruct();

	// 초기화: 전부 회색
	SetLightColor(Image_Light_Left, FLinearColor::Gray);
	SetLightColor(Image_Light_Center, FLinearColor::Gray);
	SetLightColor(Image_Light_Right, FLinearColor::Gray);


	// 초기화 시켜놓기
	ATFMatchPlayerState* PlayerState = GetPlayerState();
	if (IsValid(PlayerState))
	{
		PlayerState->OnRoundResultChanged.AddDynamic(this, &URoundIndicator::OnRoundResultChanged);
		OnRoundResultChanged(PlayerState->GetRoundResults());
	}
	else
	{
		ATFPlayerController* TFPlayerController = Cast<ATFPlayerController>(GetOwningPlayer());
		if (IsValid(TFPlayerController))
		{
			TFPlayerController->OnPlayerStateReplicated.AddUniqueDynamic(this, &URoundIndicator::OnPlayerStateInitialized);
		}
	}
}

ATFMatchPlayerState* URoundIndicator::GetPlayerState() const
{
	APlayerController* PlayerController = GetOwningPlayer();
	if (IsValid(PlayerController))
	{
		return PlayerController->GetPlayerState<ATFMatchPlayerState>();
	}
	return nullptr;
}


void URoundIndicator::OnRoundResultChanged(const TArray<bool>& RoundResults)
{
	const int32 NumRounds = RoundResults.Num();

	// Round 1
	if (NumRounds == 1)
	{
		if (RoundResults[0])
		{
			SetLightColor(Image_Light_Left, FLinearColor::Green);
		}
		else
		{
			SetLightColor(Image_Light_Right, FLinearColor::Red);
		}
	}
	else if (NumRounds == 2)	// Round 2
	{
		if (RoundResults[0] && RoundResults[1])
		{
			SetLightColor(Image_Light_Left, FLinearColor::Green);
			SetLightColor(Image_Light_Center, FLinearColor::Green);
		}
		else if (RoundResults[0] && !RoundResults[1])
		{
			SetLightColor(Image_Light_Left, FLinearColor::Green);
			SetLightColor(Image_Light_Right, FLinearColor::Red);
		}
		else if (!RoundResults[0] && RoundResults[1])
		{
			SetLightColor(Image_Light_Right, FLinearColor::Red);
			SetLightColor(Image_Light_Left, FLinearColor::Green);
		}
		else
		{
			SetLightColor(Image_Light_Right, FLinearColor::Red);
			SetLightColor(Image_Light_Center, FLinearColor::Red);
		}
	}
	else if (NumRounds >= 3)	// Round 3
	{
		SetLightColor(Image_Light_Center, RoundResults[2] ? FLinearColor::Green : FLinearColor::Red);
	}
}

void URoundIndicator::SetLightColor(UImage* Light, const FLinearColor& Color)
{
	if (Light)
	{
		Light->SetColorAndOpacity(Color);
	}
}


void URoundIndicator::OnPlayerStateInitialized()
{
	// Get the PlayerState and bind to the score changed delegate
	ATFMatchPlayerState* PlayerState = GetPlayerState();
	if (IsValid(PlayerState))
	{
		PlayerState->OnRoundResultChanged.AddDynamic(this, &URoundIndicator::OnRoundResultChanged);
		OnRoundResultChanged(PlayerState->GetRoundResults());
	}

	// Unsubscribe from the OnPlayerStateChanged delegate
	ATFPlayerController* TFPlayerController = Cast<ATFPlayerController>(GetOwningPlayer());
	if (IsValid(TFPlayerController))
	{
		TFPlayerController->OnPlayerStateReplicated.RemoveDynamic(this, &URoundIndicator::OnPlayerStateInitialized);
	}
}