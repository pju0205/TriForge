// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Types/DSTypes.h"
#include "DSGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API ADSGameModeBase : public AGameMode
{
	GENERATED_BODY()

protected:
	ADSGameModeBase();

	virtual void BeginPlay() override;
	
	void StartCountdownTimer(FCountdownTimerHandle& CountdownTimerHandle);
	void StopCountdownTimer(FCountdownTimerHandle& CountdownTimerHandle);
	void UpdateCountdownTimer(const FCountdownTimerHandle& CountdownTimerHandle);
	void TrySeamlessTravel(TSoftObjectPtr<UWorld> DestinationMap);
	void RemovePlayerSession(AController* Exiting);
	
	virtual void OnCountdownTimerFinished(ECountdownTimerType Type);
	
	virtual void NextRandomTravelMap();

	// 이동할 Combat Map 목록
	UPROPERTY(EditDefaultsOnly, Category = "Maps")
	TArray<TSoftObjectPtr<UWorld>> CombatMaps;

	// 사용한 맵 목록 (SoftPath 기준으로 비교)
	UPROPERTY()
	TSet<FSoftObjectPath> UsedCombatMapPaths;

private:
	FRandomStream MapRandomStream;
};
