// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoundIndicator.generated.h"

class ATFMatchPlayerState;
class UImage;
/**
 * 
 */
UCLASS()
class TRIFORGE_API URoundIndicator : public UUserWidget
{
	GENERATED_BODY()
public:
		
	UPROPERTY(meta = (BindWidget))
	UImage* Image_Light_Left;

	UPROPERTY(meta = (BindWidget))
	UImage* Image_Light_Center;

	UPROPERTY(meta = (BindWidget))
	UImage* Image_Light_Right;
	
	
	UFUNCTION(BlueprintCallable)
	void OnRoundResultChanged(const TArray<bool>& RoundResults);

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnPlayerStateInitialized();

	ATFMatchPlayerState* GetPlayerState() const;

private:
	void SetLightColor(UImage* Light, const FLinearColor& Color);
};