// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHealthBar.generated.h"

class UTFPlayerHealthComponent;
class UTextBlock;
class UProgressBar;
/**
 * 
 */
UCLASS()
class TRIFORGE_API UPlayerHealthBar : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void SetHealthBar();
	
	UFUNCTION()
	void UpdateHealthBar(float Health, float MaxHealth);

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthText;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UTFPlayerHealthComponent* GetPlayerComp() const;

	void BindToHealthComponent(UTFPlayerHealthComponent* NewComp);
	
	UFUNCTION()
	void OnHealthCompInitialized();
protected:
	

private:
	UPROPERTY()
	UTFPlayerHealthComponent* CachedHealthComponent = nullptr;
};
