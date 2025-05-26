// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Portal/Dashboard/Game/GameListCard.h"

#include "Components/TextBlock.h"

void UGameListCard::SetSessionInfo(const FString& SessionName, int32 Player, int32 MaxPlayer) const
{
	TextBlock_SessionName->SetText(FText::FromString(SessionName));
	TextBlock_Players->SetText(FText::FromString(Player + "/" + MaxPlayer));
}
