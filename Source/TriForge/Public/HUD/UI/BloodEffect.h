// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BloodEffect.generated.h"

/**
 * 
 */
UCLASS()
class TRIFORGE_API UBloodEffect : public UUserWidget
{
	GENERATED_BODY()
public:
	// 애니메이션 바인딩
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* DamageFlashAnim;

	void PlayDamageAnimation();
};
