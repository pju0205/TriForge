// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/UI/RoundIndicator.h"

#include "Components/Image.h"

void URoundIndicator::NativeConstruct()
{
	Super::NativeConstruct();

	// 초기화: 전부 회색
	SetLightColor(Image_Light_Left, FLinearColor::Gray);
	SetLightColor(Image_Light_Center, FLinearColor::Gray);
	SetLightColor(Image_Light_Right, FLinearColor::Gray);
}

void URoundIndicator::SetRoundResults(const TArray<bool>& RoundResults)
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
	else if (NumRounds == 2)
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
	else if (NumRounds >= 3)
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
