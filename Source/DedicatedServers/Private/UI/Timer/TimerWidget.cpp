// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Timers/TimerWidget.h"

#include "Components/TextBlock.h"
#include "Kismet/KismetStringLibrary.h"
#include "Player/DSPlayerController.h"



void UTimerWidget::NativeOnInitialized()
{
 	Super::NativeOnInitialized();

	// 소유 컨트롤러 가져오기
 	OwningPlayerController = Cast<ADSPlayerController>(GetOwningPlayer());
 	if (IsValid(OwningPlayerController))
 	{
 		// 타이머 이벤트 델리게이트 바인딩
 		OwningPlayerController->OnTimerUpdated.AddDynamic(this, &UTimerWidget::OnTimerUpdated);
 		OwningPlayerController->OnTimerStopped.AddDynamic(this, &UTimerWidget::OnTimerStopped);
 	}
	
	// 비활성일 경우 타이머 텍스트 숨기기
 	if (bHiddenWhenInactive)
 	{
 		TextBlock_Time->SetRenderOpacity(0.f);
 	}
}
 
void UTimerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
 	Super::NativeTick(MyGeometry, InDeltaTime);

	// 타이머가 활성 상태일 경우 시간 감소 및 갱신
 	if (bActive)
 	{
 		UpdateCountdown(InternalCountdown - InDeltaTime);
 	}
}
 
void UTimerWidget::OnTimerUpdated(float CountdownTimeLeft, ECountdownTimerType Type)
{
	// 설정된 타입과 일치하지 않으면 무시
 	if (Type != TimerType) return;

	// 최초 업데이트라면 타이머 시작 처리
 	if (!bActive)
 	{
 		TimerStarted(CountdownTimeLeft);
 	}

	// 시간 갱신 및 BP 이벤트 호출
 	UpdateCountdown(CountdownTimeLeft);
 	K2_OnTimerUpdated(CountdownTimeLeft, TimerType);
}
 
void UTimerWidget::OnTimerStopped(float CountdownTimeLeft, ECountdownTimerType Type)
{
 	if (Type != TimerType) return;

	// 정지 처리 및 BP 이벤트 호출
 	TimerStopped();
 	K2_OnTimerStopped(CountdownTimeLeft, TimerType);
}
 
FString UTimerWidget::FormatTimeAsString(float TimeSeconds) const
{
	// 음수 허용 안 하면 절댓값으로 표시
 	TimeSeconds = bCanBeNegative ? TimeSeconds : FMath::Abs(TimeSeconds);
	
 	FString DisplayTimeString;
 	if (bShowCentiSeconds)
 	{
 		// 소수점 포함 형식 ("00:00.00")으로 표시
 		DisplayTimeString = UKismetStringLibrary::TimeSecondsToString(TimeSeconds);
 	}
 	else
 	{
 		// 정수 분:초 포맷 ("00:00")으로 표시
 		const TCHAR* NegativeModifier = TimeSeconds < 0.f? TEXT("-") : TEXT("");
 		TimeSeconds = FMath::Abs(TimeSeconds);
 		const int32 NumMinutes = FMath::FloorToInt(TimeSeconds/60.f);
 		const int32 NumSeconds = FMath::FloorToInt(TimeSeconds-(NumMinutes*60.f));
 		DisplayTimeString = FString::Printf(TEXT("%s%02d:%02d"), NegativeModifier, NumMinutes, NumSeconds);
 	}
	
 	return DisplayTimeString;
}
 
void UTimerWidget::TimerStarted(float InitialTime)
{
 	bActive = true;
 	TextBlock_Time->SetRenderOpacity(1.f);	// 보이게 처리

	K2_OnTimerStarted(InitialTime, TimerType);
}

void UTimerWidget::TimerStopped()
{
 	bActive = false;
 	UpdateCountdown(0.f);		// 0초로 리셋
 	if (bHiddenWhenInactive)
 	{
 		TextBlock_Time->SetRenderOpacity(0.f);	// 안보이게 숨김
 	}
}
 
void UTimerWidget::UpdateCountdown(float TimeSeconds)
{
 	InternalCountdown = TimeSeconds;
 	const FText TimeText = FText::FromString(FormatTimeAsString(InternalCountdown));
 	TextBlock_Time->SetText(TimeText);	// 텍스트 갱신
}