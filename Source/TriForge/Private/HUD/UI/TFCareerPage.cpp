// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/UI/TFCareerPage.h"

#include "Components/ScrollBox.h"
#include "Data/TFPlayerData.h"
#include "Types/TFTypes.h"
#include "UI/HTTP/HTTPRequestTypes.h"
#include "UI/Portal/Dashboard/Career/CareerAchievement.h"

void UTFCareerPage::OnRetrieveMatchStats(const FDSRetrieveMatchStatsResponse& RetrieveMatchStatsResponse)
{
	Super::OnRetrieveMatchStats(RetrieveMatchStatsResponse);
	
	ScrollBox_Achievements->ClearChildren();

	TMap<ETFPlayerType, int32> AchievementData;
	
	if (RetrieveMatchStatsResponse.kills > 0) AchievementData.Emplace(ETFPlayerType::Kills, RetrieveMatchStatsResponse.kills);
	if (RetrieveMatchStatsResponse.deaths > 0) AchievementData.Emplace(ETFPlayerType::Deaths, RetrieveMatchStatsResponse.deaths);
	if (RetrieveMatchStatsResponse.roundScore > 0) AchievementData.Emplace(ETFPlayerType::RoundScore, RetrieveMatchStatsResponse.roundScore);
	if (RetrieveMatchStatsResponse.matchScore > 0) AchievementData.Emplace(ETFPlayerType::MatchScore, RetrieveMatchStatsResponse.matchScore);
	// if (RetrieveMatchStatsResponse.matchWins > 0) AchievementData.Emplace(ETFPlayerType::Wins, RetrieveMatchStatsResponse.matchWins);
	// if (RetrieveMatchStatsResponse.matchLosses > 0) AchievementData.Emplace(ETFPlayerType::Losess, RetrieveMatchStatsResponse.matchWins);
	
	check(TFPlayerData);
	
	// Achievement 카드 붙이기
	for (const TPair<ETFPlayerType, int32>& Pair : AchievementData)
	{
		const FString& CareerAchievementName = TFPlayerData->TFPlayerInfo.FindChecked(Pair.Key).CareerPageAchievementName;
		UTexture2D* Icon = TFPlayerData->TFPlayerInfo.FindChecked(Pair.Key).ElimIcon;

		UCareerAchievement* CareerAchievement = CreateWidget<UCareerAchievement>(this, CareerAchievementClass);
		UE_LOG(LogTemp, Warning, TEXT("CareerAchievement 생성됨? %s"), CareerAchievement ? TEXT("Yes") : TEXT("No"));
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
