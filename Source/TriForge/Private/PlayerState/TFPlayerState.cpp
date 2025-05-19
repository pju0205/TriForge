

#include "PlayerState/TFPlayerState.h"


ATFPlayerState::ATFPlayerState()
{
	PrimaryActorTick.bCanEverTick = true;
	NetUpdateFrequency = 10.f;
}

void ATFPlayerState::BeginPlay()
{
	Super::BeginPlay();
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