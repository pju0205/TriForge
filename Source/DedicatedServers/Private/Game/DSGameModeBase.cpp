// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/DSGameModeBase.h"

#include "Kismet/GameplayStatics.h"
#include "Player/DSPlayerController.h"
#include "GameLiftServerSDK.h"

ADSGameModeBase::ADSGameModeBase()
{
	bUseSeamlessTravel = true;
}

void ADSGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// Map 랜덤성 부여를 위한 값
	MapRandomStream.Initialize(FDateTime::Now().GetMillisecond());
}

// 카운트 다운 타이머
void ADSGameModeBase::StartCountdownTimer(FCountdownTimerHandle& CountdownTimerHandle)
{
	CountdownTimerHandle.TimerFinishedDelegate.BindWeakLambda(this, [&]()
	{
		OnCountdownTimerFinished(CountdownTimerHandle.Type);
	});

	GetWorldTimerManager().SetTimer(
		CountdownTimerHandle.TimerFinishedHandle,
		CountdownTimerHandle.TimerFinishedDelegate,
		CountdownTimerHandle.CountdownTime,
		false);

	CountdownTimerHandle.TimerUpdateDelegate.BindWeakLambda(this, [&]()
	{
		UpdateCountdownTimer(CountdownTimerHandle);
	});

	GetWorldTimerManager().SetTimer(
		CountdownTimerHandle.TimerUpdateHandle,
		CountdownTimerHandle.TimerUpdateDelegate,
		CountdownTimerHandle.CountdownUpdateInterval,
		true);
	UpdateCountdownTimer(CountdownTimerHandle);
}


// 타이머 멈추기
void ADSGameModeBase::StopCountdownTimer(FCountdownTimerHandle& CountdownTimerHandle)
{
	CountdownTimerHandle.State = ECountdownTimerState::Stopped;
	GetWorldTimerManager().ClearTimer(CountdownTimerHandle.TimerFinishedHandle);
	GetWorldTimerManager().ClearTimer(CountdownTimerHandle.TimerUpdateHandle);
	if (CountdownTimerHandle.TimerFinishedDelegate.IsBound())
	{
		CountdownTimerHandle.TimerFinishedDelegate.Unbind();
	}
	if (CountdownTimerHandle.TimerUpdateDelegate.IsBound())
	{
		CountdownTimerHandle.TimerUpdateDelegate.Unbind();
	}

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		ADSPlayerController* DSPlayerController = Cast<ADSPlayerController>(Iterator->Get());
		if (IsValid(DSPlayerController))
		{
			DSPlayerController->Client_TimerStopped(0.f, CountdownTimerHandle.Type);
		}
	}
}

void ADSGameModeBase::UpdateCountdownTimer(const FCountdownTimerHandle& CountdownTimerHandle)
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		ADSPlayerController* DSPlayerController = Cast<ADSPlayerController>(Iterator->Get());
		if (IsValid(DSPlayerController))
		{
			const float CountdownTimeLeft = CountdownTimerHandle.CountdownTime - GetWorldTimerManager().GetTimerElapsed(CountdownTimerHandle.TimerFinishedHandle);
			DSPlayerController->Client_TimerUpdated(CountdownTimeLeft, CountdownTimerHandle.Type);
		}
	}
}

void ADSGameModeBase::OnCountdownTimerFinished(ECountdownTimerType Type)
{
 	
}

void ADSGameModeBase::NextRandomTravelMap()
{
	if (CombatMaps.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("CombatMaps 배열이 비어있음."));
		return;
	}

	// 사용 가능한 맵만 필터링
	TArray<TSoftObjectPtr<UWorld>> AvailableMaps;
	for (const TSoftObjectPtr<UWorld>& Map : CombatMaps)
	{
		if (!UsedCombatMapPaths.Contains(Map.ToSoftObjectPath()))
		{
			AvailableMaps.Add(Map);
		}
	}
	
	// 모든 맵이 사용되었으면 초기화 (루프 가능)
	if (AvailableMaps.Num() == 0)
	{
		UE_LOG(LogTemp, Display, TEXT("모든 CombatMap을 사용했으므로 목록 초기화"));
		UsedCombatMapPaths.Empty();
		AvailableMaps = CombatMaps;
	}

	// 랜덤 선택
	int32 RandomIndex = MapRandomStream.RandRange(0, AvailableMaps.Num() - 1);
	TSoftObjectPtr<UWorld> SelectedMap = AvailableMaps[RandomIndex];

	if (SelectedMap.IsValid() || SelectedMap.ToSoftObjectPath().IsValid())
	{
		UsedCombatMapPaths.Add(SelectedMap.ToSoftObjectPath());

		// 맵 이동
		TrySeamlessTravel(SelectedMap);
		UE_LOG(LogTemp, Display, TEXT("Map Index: %d, Path: %s"), RandomIndex, *SelectedMap.ToString());
	}
}

// Map 이동 함수
void ADSGameModeBase::TrySeamlessTravel(TSoftObjectPtr<UWorld> DestinationMap)
{
	const FString MapName = DestinationMap.ToSoftObjectPath().GetAssetName();
	if (GIsEditor)	// Editor에서 실행하는 코드의 경우
	{
		UGameplayStatics::OpenLevelBySoftObjectPtr(this, DestinationMap);
	}
	else
	{
		FString TravelURL = MapName + TEXT("?SeamlessTravel");
		GetWorld()->ServerTravel(MapName, true);
	}
}

// playerSession 제거 함수
void ADSGameModeBase::RemovePlayerSession(AController* Exiting)
{
	ADSPlayerController* DSPlayerController = Cast<ADSPlayerController>(Exiting);
	if (!IsValid(DSPlayerController)) return;
 
#if WITH_GAMELIFT
	const FString& PlayerSessionId = DSPlayerController->PlayerSessionId;
	if (!PlayerSessionId.IsEmpty())
	{
		Aws::GameLift::Server::RemovePlayerSession(TCHAR_TO_ANSI(*PlayerSessionId));
	}
#endif
}