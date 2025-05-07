// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/DSTypes.h"
#include "TimerWidget.generated.h"

class ADSPlayerController;
class UTextBlock;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UTimerWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
 
protected:
 
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	ECountdownTimerType TimerType = ECountdownTimerType::None;
 
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ADSPlayerController> OwningPlayerController;
 
	UPROPERTY(BlueprintReadOnly)
	bool bActive = false;


	// 해당 부분은 선택 가능
	UPROPERTY(EditDefaultsOnly)
	bool bCanBeNegative = false;	// 음수 표시할지 선택
 
	UPROPERTY(EditDefaultsOnly)
	bool bShowCentiSeconds = true;  // 소수점 자리 표시할지 선택
 
	UPROPERTY(EditDefaultsOnly)
	bool bHiddenWhenInactive = true; // 카운트 다운 끝나고 숨길지 선택
	// ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
	
	UFUNCTION()
	virtual void OnTimerUpdated(float CountdownTimeLeft, ECountdownTimerType Type);
 
	UFUNCTION()
	virtual void OnTimerStopped(float CountdownTimeLeft, ECountdownTimerType Type);
 
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Timer Updated"))
	void K2_OnTimerUpdated(float Time, ECountdownTimerType Type);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Timer Started"))
	void K2_OnTimerStarted(float Time, ECountdownTimerType Type);
 
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Timer Stopped"))
	void K2_OnTimerStopped(float Time, ECountdownTimerType Type);
 
	FString FormatTimeAsString(float TimeSeconds) const;
private:
 
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Time;
 
	float InternalCountdown;
 
	void TimerStarted(float InitialTime);
	void TimerStopped();
	void UpdateCountdown(float TimeSeconds);
};