// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TFPlayerController.h"
#include "Character/TFCharacter.h"
#include "Character/TFAnimInstance.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "HUD/TFHUD.h"
#include "HUD/TFOverlay.h"
#include "PlayerState/TFPlayerState.h"

ATFPlayerController::ATFPlayerController()
{
	bQuitMenuOpen = false;	// Quit 버튼 상태
}

void ATFPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->AddMappingContext(TFCharacterContext, 0);
	}

	TFHUD = Cast<ATFHUD>(GetHUD());
}

void ATFPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	ATFPlayerState* TFPlayerState = Cast<ATFPlayerState>(InPawn->GetPlayerState());
	if (TFPlayerState)
	{
		SetHUDHealth(TFPlayerState->GetCurrentHealth(), TFPlayerState->GetMaxHealth());
	}
}

void ATFPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATFPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATFPlayerController::Move);
	EnhancedInputComponent->BindAction(RotationAction, ETriggerEvent::Triggered, this, &ATFPlayerController::Rotation);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ATFPlayerController::Jump);
	EnhancedInputComponent->BindAction(SlideAction, ETriggerEvent::Started, this, &ATFPlayerController::Slide);
	// EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ATFPlayerController::CrouchStart);
	// EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ATFPlayerController::CrouchEnd);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ATFPlayerController::SprintStart);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ATFPlayerController::SprintEnd);
	EnhancedInputComponent->BindAction(QuitAction, ETriggerEvent::Started, this, &ATFPlayerController::Input_Quit);	// Quit 버튼


	EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ATFPlayerController::AimingStarted);	
	EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ATFPlayerController::AimingReleased);	
	EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &ATFPlayerController::EquipWeapon);
	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ATFPlayerController::WeaponAttackStarted);
	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &ATFPlayerController::WeaponAttackReleased);
}


void ATFPlayerController::Move(const struct FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{

		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);

		MoveDir = InputAxisVector;
	}
	
}

void ATFPlayerController::Rotation(const FInputActionValue& InputActionValue)
{
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		const FVector2D RotationValue = InputActionValue.Get<FVector2D>();

		ControlledPawn->AddControllerYawInput(RotationValue.X);
		ControlledPawn->AddControllerPitchInput(RotationValue.Y);
		// AddYawInput(RotationValue.X);
		// AddPitchInput(RotationValue.Y);

	}
}

void ATFPlayerController::SprintStart(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ATFPlayerCharacter* TFCharacter = Cast<ATFPlayerCharacter>(ControlledPawn);
		if (TFCharacter)
		{
			TFCharacter->UpdateSprintState(true);
		}
	}
}

void ATFPlayerController::SprintEnd(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ATFPlayerCharacter* TFCharacter = Cast<ATFPlayerCharacter>(ControlledPawn);
		if (TFCharacter)
		{
			TFCharacter->UpdateSprintState(false);
		}
	}
}

void ATFPlayerController::Jump(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ATFPlayerCharacter* TFCharacter = Cast<ATFPlayerCharacter>(ControlledPawn);
		if (TFCharacter)
		{
			TFCharacter->Jump();
		}
	}
}

void ATFPlayerController::Slide(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ATFPlayerCharacter* TFCharacter = Cast<ATFPlayerCharacter>(ControlledPawn);
		if (TFCharacter)
		{
			TFCharacter->isPlayingSlideMontage(MoveDir.Y, MoveDir.X);
		}
	}
}
//
//
// void ATFPlayerController::CrouchStart(const FInputActionValue& InputActionValue)
// {
//
// 	if (APawn* ControlledPawn = GetPawn<APawn>())
// 	{
// 		ATFCharacter* MyCharacter = Cast<ATFCharacter>(ControlledPawn);
// 		if (MyCharacter)
// 		{
// 			MyCharacter->UpdateCrouchState(true);
// 		}
// 	}
// }
//
// void ATFPlayerController::CrouchEnd(const FInputActionValue& InputActionValue)
// {
//
// 	if (APawn* ControlledPawn = GetPawn<APawn>())
// 	{
// 		ATFCharacter* MyCharacter = Cast<ATFCharacter>(ControlledPawn);
// 		if (MyCharacter)
// 		{
// 			MyCharacter->UpdateCrouchState(false);
// 		}
// 	}
// }

void ATFPlayerController::Input_Quit()
{
	bQuitMenuOpen = !bQuitMenuOpen;
	if (bQuitMenuOpen)
	{
		FInputModeGameAndUI InputMode;
		SetInputMode(InputMode);
		SetShowMouseCursor(true);
		OnQuitMenuOpen.Broadcast(true);
	}
	else
	{
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
		SetShowMouseCursor(false);
		OnQuitMenuOpen.Broadcast(false);
	}
}

void ATFPlayerController::AimingStarted(const struct FInputActionValue& AimActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ATFPlayerCharacter* TFCharacter = Cast<ATFPlayerCharacter>(ControlledPawn);
		if (TFCharacter)
		{
			
		}
	}
}

void ATFPlayerController::AimingReleased(const struct FInputActionValue& AimActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ATFPlayerCharacter* TFCharacter = Cast<ATFPlayerCharacter>(ControlledPawn);
		if (TFCharacter)
		{
			
		}
	}
}

void ATFPlayerController::EquipWeapon(const struct FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ATFPlayerCharacter* TFCharacter = Cast<ATFPlayerCharacter>(ControlledPawn);
		if (TFCharacter)
		{
			TFCharacter->EquipButtonPressed();
		}
	}
}

void ATFPlayerController::WeaponAttackStarted(const struct FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ATFPlayerCharacter* TFCharacter = Cast<ATFPlayerCharacter>(ControlledPawn);
		if (TFCharacter)
		{
			TFCharacter->AttackButtonPressed();
		}
	}
}

void ATFPlayerController::WeaponAttackReleased(const struct FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ATFPlayerCharacter* TFCharacter = Cast<ATFPlayerCharacter>(ControlledPawn);
		if (TFCharacter)
		{
			TFCharacter->AttackButtonReleased();
		}
	}
}

void ATFPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	TFHUD = TFHUD == nullptr ? Cast<ATFHUD>(GetHUD()) : TFHUD;
	
	bool bTFHUDValid = TFHUD && TFHUD->CharacterOverlay && TFHUD->CharacterOverlay->HealthBar && TFHUD->CharacterOverlay->HealthText;
	// 추후 디버깅 시 조건들 중 무엇이 false여서 bTFHUDValid가 false인지 정확히 파악하기 힘들지만
	// 일단 코드 가독성을 위해 사용
	if (bTFHUDValid)
	{
		const float HealthPercent = Health / MaxHealth;
		TFHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);

		FString HealthText = FString::Printf(TEXT("%d / %d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		TFHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
}

void ATFPlayerController::SetHUDAmmo(int32 Ammo)
{
	TFHUD = TFHUD == nullptr ? Cast<ATFHUD>(GetHUD()) : TFHUD;
	
	bool bTFHUDValid = TFHUD && TFHUD->CharacterOverlay && TFHUD->CharacterOverlay->AmmoAmount;

	if (bTFHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		TFHUD->CharacterOverlay->AmmoAmount->SetText(FText::FromString(AmmoText));
	}
}


