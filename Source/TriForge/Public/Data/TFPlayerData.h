// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Types/TFTypes.h"
#include "TFPlayerData.generated.h"


USTRUCT(BlueprintType)
struct FTFPlayerInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	ETFPlayerType ElimType = ETFPlayerType::None;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString ElimMessage = FString();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString CareerPageAchievementName = FString();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> ElimIcon = nullptr;

	UPROPERTY(BlueprintReadOnly)
	int32 SequentialElimCount = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 StreakCount = 0;
};
/**
 * 
 */
UCLASS()
class TRIFORGE_API UTFPlayerData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "playerRecordInfo")
	TMap<ETFPlayerType, FTFPlayerInfo> TFPlayerInfo;
};
