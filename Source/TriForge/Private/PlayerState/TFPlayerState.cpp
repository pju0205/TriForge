

#include "PlayerState/TFPlayerState.h"

#include "Character/TFWeaponPlayerController.h"
#include "Net/UnrealNetwork.h"

ATFPlayerState::ATFPlayerState()
{
	NetUpdateFrequency = 10.f;
}

void ATFPlayerState::BeginPlay()
{
	Super::BeginPlay();

	TFPlayerController = TFPlayerController == nullptr ? Cast<ATFWeaponPlayerController>(GetPlayerController()) : TFPlayerController;
	TFPlayerController->SetHUDHealth(CurrentHealth, MaxHealth);
}

void ATFPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATFPlayerState, CurrentHealth);
}

void ATFPlayerState::OnRep_Health()
{
	ATFWeaponPlayerController* PlayerController = Cast<ATFWeaponPlayerController>(GetPlayerController());
	if (PlayerController)
	{
		PlayerController->SetHUDHealth(CurrentHealth, MaxHealth);
	}
}

void ATFPlayerState::CalcDamage(float Damage)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);
}
