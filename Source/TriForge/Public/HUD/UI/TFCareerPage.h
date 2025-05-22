// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Portal/Dashboard/CareerPage.h"
#include "TFCareerPage.generated.h"

class UTFPlayerData;
/**
 * 
 */
UCLASS()
class TRIFORGE_API UTFCareerPage : public UCareerPage
{
	GENERATED_BODY()

public:
	virtual void OnRetrieveMatchStats(const FDSRetrieveMatchStatsResponse& RetrieveMatchStatsResponse) override;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTFPlayerData> TFPlayerData;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCareerAchievement> CareerAchievementClass;
};
