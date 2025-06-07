
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TFOverlay.generated.h"

class UBloodEffect;
class UImage;
class UChatting;
class UPlayerHealthBar;
class UMatchResultPage;
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
	UPROPERTY(meta = (BindWidget))
	UPlayerHealthBar* PlayerHealthBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AmmoAmount;

	UPROPERTY(meta = (BindWidget))
	UImage* WeaponImage;

	UPROPERTY(meta = (BindWidget))
	URoundIndicator* RoundIndicator;

	UPROPERTY(meta = (BindWidget))
	UMatchResultPage* MatchResultWidget;

	UPROPERTY(meta = (BindWidget))
	UChatting* ChatPanel;

	UPROPERTY(meta = (BindWidget))
	UBloodEffect* BloodEffect;
};
