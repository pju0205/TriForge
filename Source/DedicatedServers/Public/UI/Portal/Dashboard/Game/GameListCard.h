// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameListCard.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCardSelected, const FString&, SelectedSession);
class UButton;
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
	
	UPROPERTY(BlueprintAssignable)
	FOnCardSelected OnCardSelected;

	// 이 부분 추가됨
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Session;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_SessionName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Players;
	
	FString GameSessionId;

	void SetSelected(bool bSelected);
protected:
	virtual void NativeOnInitialized() override;

private:
	UFUNCTION()
	void HandleClick();

	bool bIsSelected = false;
};
