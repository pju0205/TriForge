

#include "Character/TFWeaponPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/TFWeaponCharacter.h"

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
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("Playercontroller")));
			}
		}
	}
}

