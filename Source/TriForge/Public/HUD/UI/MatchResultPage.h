// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MatchResultPage.generated.h"

class ATFMatchPlayerState;
class UTextBlock;
/**
 * 
 */
UCLASS()
class TRIFORGE_API UMatchResultPage : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
		
	UFUNCTION(BlueprintCallable)
	void UpdateResultData(bool bIsWinner, int32 LeftScore, int32 RightScore);

	UFUNCTION(BlueprintCallable)
	void SetMatchResultName();

	void OnMatchResultChanged(bool bIsWinner, int32 LeftScore, int32 RightScore);

	UFUNCTION()
	void HideResultPage();

protected:
	UPROPERTY(meta = (BindWidget)) UTextBlock* ResultText;
	
	UPROPERTY(meta = (BindWidget)) UTextBlock* LeftNameText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* LeftScoreText;
	
	UPROPERTY(meta = (BindWidget)) UTextBlock* RightNameText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* RightScoreText;

	UPROPERTY(EditDefaultsOnly)
	float RateTime = 9;

	ATFMatchPlayerState* GetPlayerState() const;

	UFUNCTION()
	void OnPlayerStateInitialized();

private:

	FString LeftName;
	FString RightName;
};
