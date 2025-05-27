// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePage.generated.h"


struct FDSGameSession;
class UButton;
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

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RefreshButtonWidget;			// 추가
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameSessionsManager> GameSessionsManagerClass;

	// 게임 리스트 표시용
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> ScrollBox_GameList;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameListCard> GameListCardClass;
	
	UFUNCTION()
	void UpdateSessionListUI( const FDSGameSession& GameSession);

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

	UFUNCTION()
	void RefreshButtonClicked();

	UFUNCTION()
	void HandleCardSelected(const FString& GameSessionID);

	UPROPERTY()
	UGameListCard* SelectedCard = nullptr;
	
	FString SelectedGameSessionID;
	bool bHasSelectedSession = false;
 
	UPROPERTY()
	TObjectPtr<UGameSessionsManager> GameSessionsManager;
};
