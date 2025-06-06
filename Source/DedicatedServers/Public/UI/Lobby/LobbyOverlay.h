// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyOverlay.generated.h"


class UChatting;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API ULobbyOverlay : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	UChatting* ChatPanel;
};
