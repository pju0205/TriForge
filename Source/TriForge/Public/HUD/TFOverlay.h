
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TFOverlay.generated.h"

class URoundIndicator;
class UTextBlock;
class UProgressBar;
/**
 * 
 */
UCLASS()
class TRIFORGE_API UTFOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta =(BindWidget))
	UProgressBar* HealthBar;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AmmoAmount;

	UPROPERTY(meta = (BindWidget))
	URoundIndicator* RoundIndicator;


	UFUNCTION()
	void UpdateHealthBar(float Health, float MaxHealth);
};
