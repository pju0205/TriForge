

#include "Character/TFWeaponPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/TFWeaponCharacter.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "HUD/TFHUD.h"
#include "HUD/TFOverlay.h"

ATFWeaponPlayerController::ATFWeaponPlayerController()
{
	
}

void ATFWeaponPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->AddMappingContext(CharacterContext, 0);
	}

	TFHUD = Cast<ATFHUD>(GetHUD());
}

void ATFWeaponPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	
}

void ATFWeaponPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATFWeaponPlayerController::Move);
	EnhancedInputComponent->BindAction(RotationAction, ETriggerEvent::Triggered, this, &ATFWeaponPlayerController::Rotation);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ATFWeaponPlayerController::Jump);

	
	EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &ATFWeaponPlayerController::EquipWeapon);
	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ATFWeaponPlayerController::WeaponAttackStarted);
	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &ATFWeaponPlayerController::WeaponAttackReleased);
}

void ATFWeaponPlayerController::Move(const struct FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y); // W/S 키
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);    // A/D 키
	}
}

void ATFWeaponPlayerController::Rotation(const struct FInputActionValue& InputActionValue)
{
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		const FVector2D RotationValue = InputActionValue.Get<FVector2D>();

		AddYawInput(RotationValue.X);
		AddPitchInput(RotationValue.Y);
	}
}

void ATFWeaponPlayerController::Jump(const struct FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ATFWeaponCharacter* TFCharacter = Cast<ATFWeaponCharacter>(ControlledPawn);
		if (TFCharacter)
		{
			TFCharacter->Jump();
		}
	}
}

void ATFWeaponPlayerController::EquipWeapon(const struct FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ATFWeaponCharacter* TFCharacter = Cast<ATFWeaponCharacter>(ControlledPawn);
		if (TFCharacter)
		{
			TFCharacter->EquipButtonPressed();
		}
	}
}

void ATFWeaponPlayerController::WeaponAttackStarted(const struct FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ATFWeaponCharacter* TFCharacter = Cast<ATFWeaponCharacter>(ControlledPawn);
		if (TFCharacter)
		{
			TFCharacter->AttackButtonPressed();
		}
	}
}

void ATFWeaponPlayerController::WeaponAttackReleased(const struct FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ATFWeaponCharacter* TFCharacter = Cast<ATFWeaponCharacter>(ControlledPawn);
		if (TFCharacter)
		{
			TFCharacter->AttackButtonReleased();
		}
	}
}

void ATFWeaponPlayerController::SetHUDHealth(float Health, float MaxHealth)
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
