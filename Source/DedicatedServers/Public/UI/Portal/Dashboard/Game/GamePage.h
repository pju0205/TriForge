// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePage.generated.h"


class UJoinGame;
class UGameListCard;
class UScrollBox;
class UStatusMessage;
class UTextBlock;
class UHostGame;
class UGameSessionsManager;
class UQuickMatchGame;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UGamePage : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UQuickMatchGame> QuickMatchGameWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHostGame> HostGameWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UJoinGame> JoinGameWidget;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UStatusMessage> StatusMessageWidget;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameSessionsManager> GameSessionsManagerClass;

	// 게임 리스트 표시용
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> ScrollBox_GameList;

	// UPROPERTY(meta = (BindWidget))
	// TObjectPtr<UTextBlock> TextBlock_StatusMessage;	따로 구현해둠

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameListCard> GameListCardClass;
	
	/*UFUNCTION(BlueprintCallable)
	void UpdateSessionListUI();*/

	// Host, QuickMatch, Join시 상태 메세지
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
	void QuickMatchGameButtonClicked();

	UFUNCTION()
	void JoinGameButtonClicked();
	
 
	UPROPERTY()
	TObjectPtr<UGameSessionsManager> GameSessionsManager;
};
