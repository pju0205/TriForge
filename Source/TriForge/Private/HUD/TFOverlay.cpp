

#include "HUD/TFOverlay.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UTFOverlay::UpdateHealthBar(float Health, float MaxHealth)
{
	if (HealthBar)
	{
		const float HealthPercent = MaxHealth > 0.f ? Health / MaxHealth : 0.f;
		HealthBar->SetPercent(HealthPercent);
	}

	if (HealthText)
	{
		const FString HealthString = FString::Printf(TEXT("%d / %d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		HealthText->SetText(FText::FromString(HealthString));
	}
}
