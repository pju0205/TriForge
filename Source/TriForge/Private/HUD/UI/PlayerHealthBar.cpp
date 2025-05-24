// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/UI/PlayerHealthBar.h"

#include "Character/TFPlayerController.h"
#include "Character/Component/TFPlayerHealthComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "HUD/UI/MatchResultPage.h"

void UPlayerHealthBar::NativeConstruct()
{
	Super::NativeConstruct();

	SetHealthBar();

	// 초기화 시켜놓기
	if (ATFPlayerController* TFPC = Cast<ATFPlayerController>(GetOwningPlayer()))
	{
		UTFPlayerHealthComponent* HealthComp = TFPC->GetHealthComponent();
		if (IsValid(HealthComp))
		{
			HealthComp->OnHealthChanged.AddDynamic(this, &UPlayerHealthBar::UpdateHealthBar);
			SetHealthBar();
		}
		else
		{
			TFPC->OnPlayerStateReplicated.AddUniqueDynamic(this, &UPlayerHealthBar::OnHealthCompInitialized);
		}
	}
}

void UPlayerHealthBar::SetHealthBar()
{
	UTFPlayerHealthComponent* HealthComp = GetPlayerComp();
	if (IsValid(HealthComp))
	{
		const float CurrentHealth = HealthComp->GetCurrentHealth(); // 또는 HealthComp->CurrentHealth;
		const float MaxHealth = HealthComp->GetMaxHealth();         // 또는 HealthComp->MaxHealth;
		UpdateHealthBar(CurrentHealth, MaxHealth);
	}
}

void UPlayerHealthBar::UpdateHealthBar(float Health, float MaxHealth)
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

UTFPlayerHealthComponent* UPlayerHealthBar::GetPlayerComp() const
{
	APlayerController* PlayerController = GetOwningPlayer();
	if (IsValid(PlayerController))
	{
		APawn* Pawn = PlayerController->GetPawn();
		if (IsValid(Pawn))
		{
			return Pawn->FindComponentByClass<UTFPlayerHealthComponent>();
		}
	}
	return nullptr;
}

void UPlayerHealthBar::OnHealthCompInitialized()
{
	if (ATFPlayerController* TFPC = Cast<ATFPlayerController>(GetOwningPlayer()))
	{
		UTFPlayerHealthComponent* HealthComp = TFPC->GetHealthComponent();
		if (IsValid(HealthComp))
		{
			HealthComp->OnHealthChanged.AddDynamic(this, &UPlayerHealthBar::UpdateHealthBar);
			SetHealthBar();
		}
	}

	if (ATFPlayerController* TFPC = Cast<ATFPlayerController>(GetOwningPlayer()))
	{
		UTFPlayerHealthComponent* HealthComp = TFPC->GetHealthComponent();
		if (IsValid(HealthComp))
		{
			TFPC->OnPlayerStateReplicated.RemoveDynamic(this, &UPlayerHealthBar::OnHealthCompInitialized);
		}
	}
}
