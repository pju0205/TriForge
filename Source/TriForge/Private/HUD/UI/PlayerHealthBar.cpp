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

	// 초기화 시도
	if (ATFPlayerController* TFPC = Cast<ATFPlayerController>(GetOwningPlayer()))
	{
		TFPC->OnPossessedPawnChanged.AddDynamic(this, &UPlayerHealthBar::OnPawnChanged);

		// 처음에도 시도
		OnHealthCompInitialized();
	}
}

void UPlayerHealthBar::NativeDestruct()
{
	Super::NativeDestruct();
	
	if (IsValid(CachedHealthComponent))
	{
		CachedHealthComponent->OnHealthChanged.RemoveDynamic(this, &UPlayerHealthBar::UpdateHealthBar);
	}
}

void UPlayerHealthBar::BindToHealthComponent(UTFPlayerHealthComponent* OldComp, UTFPlayerHealthComponent* NewComp)
{
	if (!IsValid(NewComp)) return;

	if (CachedHealthComponent == NewComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("BindToHealthComponent: Already bound to %s"), *GetNameSafe(NewComp));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("BindToHealthComponent: Swapping from %s to %s"), *GetNameSafe(OldComp), *GetNameSafe(NewComp));

	if (IsValid(OldComp))
	{
		OldComp->OnHealthChanged.RemoveDynamic(this, &UPlayerHealthBar::UpdateHealthBar);
	}

	CachedHealthComponent = NewComp;

	CachedHealthComponent->OnHealthChanged.AddDynamic(this, &UPlayerHealthBar::UpdateHealthBar);
	UpdateHealthBar(CachedHealthComponent->GetCurrentHealth(), CachedHealthComponent->GetMaxHealth());
}

void UPlayerHealthBar::OnPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	UTFPlayerHealthComponent* OldComp = OldPawn ? OldPawn->FindComponentByClass<UTFPlayerHealthComponent>() : nullptr;
	UTFPlayerHealthComponent* NewComp = NewPawn ? NewPawn->FindComponentByClass<UTFPlayerHealthComponent>() : nullptr;

	BindToHealthComponent(OldComp, NewComp);
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
	UTFPlayerHealthComponent* NewComp = GetPlayerComp();
	UTFPlayerHealthComponent* OldComp = CachedHealthComponent;

	UE_LOG(LogTemp, Warning, TEXT("OnHealthCompInitialized -> Old: %s / New: %s"),
		*GetNameSafe(OldComp),
		*GetNameSafe(NewComp));

	if (IsValid(NewComp))
	{
		BindToHealthComponent(OldComp, NewComp);
	}
}
