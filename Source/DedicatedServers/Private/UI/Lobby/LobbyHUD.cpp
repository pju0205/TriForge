// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Lobby/LobbyHUD.h"

#include "Blueprint/UserWidget.h"
#include "UI/Lobby/LobbyOverlay.h"

void ALobbyHUD::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* OwningPlayerController = GetOwningPlayerController();

	LobbyOverlay = CreateWidget<ULobbyOverlay>(OwningPlayerController, LobbyOverlayClass);
	if (IsValid(LobbyOverlay))
	{
		LobbyOverlay->AddToViewport();	// 뷰에 띄우기
	}

	bReplicates = true;
}
