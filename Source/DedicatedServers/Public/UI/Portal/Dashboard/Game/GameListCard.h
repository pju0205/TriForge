// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameListCard.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UGameListCard : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetSessionInfo(const FString& SessionName, int32 Player, int32 MaxPlayer) const;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_SessionName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Players;
};
