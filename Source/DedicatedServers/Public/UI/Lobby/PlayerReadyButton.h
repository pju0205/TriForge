// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerReadyButton.generated.h"

class UPlayerLabel;
class UButton;
class UTextBlock;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UPlayerReadyButton : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Ready;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Ready;

	UPROPERTY()
	UPlayerLabel* PlayerLabel;
	
	UFUNCTION()
	void OnReadyButtonClicked();

	virtual void NativeConstruct() override;
	
private:
	bool bLocalIsReady; // 클라이언트에만 있는 로컬 상태 캐시
};
