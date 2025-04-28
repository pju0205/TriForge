// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTags/DedicatedServersTags.h"
#include "APIData.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UAPIData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	FString GetAPIEndpoint(const FGameplayTag& APIEndpoint);

protected:

	// Name of this API - 데이터 자산에서 라벨링을 위한 것으로, 어떤 코드에서도 사용되지 않음
	UPROPERTY(EditDefaultsOnly)
	FString Name;
	
	UPROPERTY(EditDefaultsOnly)
	FString InvokeURL;

	UPROPERTY(EditDefaultsOnly)
	FString Stage;


	// 열거형으로 만드는 것도 좋은 선택이지만
	// gameplay Tag로 만드는게 더 유용하다.
	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, FString> Resources;
};
