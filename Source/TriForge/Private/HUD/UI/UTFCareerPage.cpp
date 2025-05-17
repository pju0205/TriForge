// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/UI/UTFCareerPage.h"

#include "Components/ScrollBox.h"
#include "Data/TFPlayerData.h"
#include "Types/TFTypes.h"
#include "UI/HTTP/HTTPRequestTypes.h"
#include "UI/Portal/Dashboard/CareerAchievement.h"

void UUTFCareerPage::OnRetrieveMatchStats(const FDSRetrieveMatchStatsResponse& RetrieveMatchStatsResponse)
{
	Super::OnRetrieveMatchStats(RetrieveMatchStatsResponse);
	
	ScrollBox_Achievements->ClearChildren();

	TMap<ETFPlayerType, int32> AchievementData;
	
	if (RetrieveMatchStatsResponse.hits > 0) AchievementData.Emplace(ETFPlayerType::Hits, RetrieveMatchStatsResponse.hits);
	if (RetrieveMatchStatsResponse.misses > 0) AchievementData.Emplace(ETFPlayerType::Misses, RetrieveMatchStatsResponse.misses);
	if (RetrieveMatchStatsResponse.roundScore > 0) AchievementData.Emplace(ETFPlayerType::RoundScore, RetrieveMatchStatsResponse.roundScore);
	if (RetrieveMatchStatsResponse.defeats > 0) AchievementData.Emplace(ETFPlayerType::Defeats, RetrieveMatchStatsResponse.defeats);
	
	check(TFPlayerData);

	for (const TPair<ETFPlayerType, int32>& Pair : AchievementData)
	{
		const FString& CareerAchievementName = TFPlayerData->TFPlayerInfo.FindChecked(Pair.Key).CareerPageAchievementName;
		UTexture2D* Icon = TFPlayerData->TFPlayerInfo.FindChecked(Pair.Key).ElimIcon;

		UCareerAchievement* CareerAchievement = CreateWidget<UCareerAchievement>(this, CareerAchievementClass);
		if (IsValid(CareerAchievement))
		{
			CareerAchievement->SetAchievementText(CareerAchievementName, Pair.Value);
			if (Icon)
			{
				CareerAchievement->SetAchievementIcon(Icon);
			}
			ScrollBox_Achievements->AddChild(CareerAchievement);
		}
	}
}
