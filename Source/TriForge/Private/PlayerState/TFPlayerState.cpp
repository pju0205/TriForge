

#include "PlayerState/TFPlayerState.h"

#include "Character/TFPlayerController.h"
#include "Net/UnrealNetwork.h"

ATFPlayerState::ATFPlayerState()
{
	PrimaryActorTick.bCanEverTick = true;
	NetUpdateFrequency = 10.f;
}

void ATFPlayerState::BeginPlay()
{
	Super::BeginPlay();
	
	TFPlayerController = TFPlayerController == nullptr ? Cast<ATFPlayerController>(GetPlayerController()) : TFPlayerController;
	if (TFPlayerController)
	{
		TFPlayerController->SetHUDHealth(CurrentHealth, MaxHealth);
	}
	
}

void ATFPlayerState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	/*TFPlayerController = TFPlayerController == nullptr ? Cast<ATFWeaponPlayerController>(GetPlayerController()) : TFPlayerController;
	if (TFPlayerController)
	{
		TFPlayerController->SetHUDHealth(CurrentHealth, MaxHealth);
	}*/
	
}

void ATFPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATFPlayerState, CurrentHealth);
}

void ATFPlayerState::OnRep_Health()
{
	TFPlayerController = TFPlayerController == nullptr ? Cast<ATFPlayerController>(GetPlayerController()) : TFPlayerController;
	if (TFPlayerController)
	{
		TFPlayerController->SetHUDHealth(CurrentHealth, MaxHealth);
	}
}

void ATFPlayerState::CalcDamage(float Damage)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);
}
