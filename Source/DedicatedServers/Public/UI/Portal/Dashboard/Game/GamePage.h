// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePage.generated.h"


class UStatusMessage;
class UTextBlock;
class UHostGame;
class UGameSessionsManager;
class UJoinGame;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UGamePage : public UUserWidget
{
	GENERATED_BODY()

public:
 
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UJoinGame> JoinGameWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHostGame> HostGameWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UStatusMessage> StatusMessageWidget;
	
 
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameSessionsManager> GameSessionsManagerClass;

	UFUNCTION()
	void SetStatusMessage(const FString& Message, bool bShouldResetWidgets);
protected:
 
	virtual void NativeConstruct() override;
 
private:
	UFUNCTION()
	void ButtonSetIsEnabled(bool bClicked);

	UFUNCTION()
	void HostGameButtonClicked();
 
	UFUNCTION()
	void JoinGameButtonClicked();
 
	UPROPERTY()
	TObjectPtr<UGameSessionsManager> GameSessionsManager;
};
