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
		TFPC->OnPawnReinitialized.AddDynamic(this, &UPlayerHealthBar::OnHealthCompInitialized);

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

void UPlayerHealthBar::BindToHealthComponent(UTFPlayerHealthComponent* NewComp)
{
	if (!IsValid(NewComp)) return;

	if (CachedHealthComponent == NewComp)
	{
		// 이미 바인딩된 상태라면 중복 방지
		UE_LOG(LogTemp, Warning, TEXT("BindToHealthComponent: NewComp is INVALID"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("BindToHealthComponent: NewComp is %s"), *GetNameSafe(NewComp));

	// 기존 바인딩 제거
	if (IsValid(CachedHealthComponent))
	{
		CachedHealthComponent->OnHealthChanged.RemoveDynamic(this, &UPlayerHealthBar::UpdateHealthBar);
	}

	CachedHealthComponent = NewComp;

	CachedHealthComponent->OnHealthChanged.AddDynamic(this, &UPlayerHealthBar::UpdateHealthBar);
	UpdateHealthBar(CachedHealthComponent->GetCurrentHealth(), CachedHealthComponent->GetMaxHealth());
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
	UTFPlayerHealthComponent* HealthComp = GetPlayerComp();
	UE_LOG(LogTemp, Warning, TEXT("OnHealthCompInitialized -> NewComp: %s"), *GetNameSafe(HealthComp));

	if (IsValid(HealthComp))
	{
		BindToHealthComponent(HealthComp);
	}
}
