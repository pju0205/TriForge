

#include "PlayerState/TFPlayerState.h"

#include "Character/TFWeaponCharacter.h"
#include "Character/TFWeaponPlayerController.h"
#include "Net/UnrealNetwork.h"

ATFPlayerState::ATFPlayerState()
{
	PrimaryActorTick.bCanEverTick = true;
	NetUpdateFrequency = 10.f;
}

void ATFPlayerState::BeginPlay()
{
	Super::BeginPlay();
	
	TFPlayerController = TFPlayerController == nullptr ? Cast<ATFWeaponPlayerController>(GetPlayerController()) : TFPlayerController;
	if (TFPlayerController)
	{
		TFPlayerController->SetHUDHealth(CurrentHealth, MaxHealth);
	}
	
}


void ATFPlayerState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TFPlayerController = TFPlayerController == nullptr ? Cast<ATFWeaponPlayerController>(GetPlayerController()) : TFPlayerController;
	if (TFPlayerController)
	{
		TFPlayerController->SetHUDHealth(CurrentHealth, MaxHealth);
	}
	
	/*if (TFPlayerController == nullptr)
	{
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			APlayerController* PC = Iterator->Get();

			APlayerState* PS = Cast<ATFPlayerState>(PC->PlayerState);
		
			if (PC && PS == this)
			{
				TFPlayerController = Cast<ATFWeaponPlayerController>(PC);
			
			}
		}
		if (TFPlayerController !=nullptr)
		{
			TFPlayerController->SetHUDHealth(CurrentHealth, MaxHealth);
		}
		else
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString(TEXT("aaaa")));
			}
		}
	}*/

	
}

void ATFPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATFPlayerState, CurrentHealth);
}

void ATFPlayerState::OnRep_Health()
{
	TFPlayerController = TFPlayerController == nullptr ? Cast<ATFWeaponPlayerController>(GetPlayerController()) : TFPlayerController;
	if (TFPlayerController)
	{
		TFPlayerController->SetHUDHealth(CurrentHealth, MaxHealth);
	}
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Black, FString::Printf(TEXT("OnRep_Health")));
	}
}

void ATFPlayerState::CalcDamage(float Damage)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);
}
