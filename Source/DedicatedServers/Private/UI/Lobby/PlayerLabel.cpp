// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Lobby/PlayerLabel.h"

#include "Components/TextBlock.h"
 
 void UPlayerLabel::SetUsername(const FString& Username) const
 {
 	TextBlock_Username->SetText(FText::FromString(Username));
 }
 
FString UPlayerLabel::GetUsername() const
 {
 	return TextBlock_Username->GetText().ToString();
 }

// Label에 있는 유저의 Ready 상태 UI 출력
void UPlayerLabel::SetReadyState(bool bIsReady)
{
 	if (TextBlock_ReadyStatus)
 	{
 		TextBlock_ReadyStatus->SetText(FText::FromString(bIsReady ? TEXT("Ready") : TEXT("Not Ready")));
 	}
}
