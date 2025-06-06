// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LobbyHUD.generated.h"

class ULobbyOverlay;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API ALobbyHUD : public AHUD
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ULobbyOverlay> LobbyOverlayClass;
	
	UPROPERTY()
	TObjectPtr<ULobbyOverlay> LobbyOverlay;
protected:

	virtual void BeginPlay() override;
};
