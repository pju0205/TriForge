// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TFPlayerController.h"
#include "Character/TFCharacter.h"
#include "Character/TFAnimInstance.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

ATFPlayerController::ATFPlayerController()
{

}

void ATFPlayerController::BeginPlay()
{
	Super::BeginPlay();
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->AddMappingContext(TFCharacterContext, 0);
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
	// EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ATFPlayerController::Jump);
	// EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ATFPlayerController::CrouchStart);
	// EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ATFPlayerController::CrouchEnd);
	// EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ATFPlayerController::SprintStart);
	// EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ATFPlayerController::SprintEnd);

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
//
// void ATFPlayerController::SprintStart(const FInputActionValue& InputActionValue)
// {
// 	if (APawn* ControlledPawn = GetPawn<APawn>())
// 	{
// 		ATFCharacter* MyCharacter = Cast<ATFCharacter>(ControlledPawn);
// 		if (MyCharacter)
// 		{
// 			MyCharacter->UpdateSprintState(true);
// 		}
// 	}
// }
//
// void ATFPlayerController::SprintEnd(const FInputActionValue& InputActionValue)
// {
// 	if (APawn* ControlledPawn = GetPawn<APawn>())
// 	{
// 		ATFCharacter* MyCharacter = Cast<ATFCharacter>(ControlledPawn);
// 		if (MyCharacter)
// 		{
// 			MyCharacter->UpdateSprintState(false);
// 		}
// 	}
// }
//
//
// void ATFPlayerController::Jump(const FInputActionValue& InputActionValue)
// {
// 	if (APawn* ControlledPawn = GetPawn<APawn>())
// 	{
// 		ATFCharacter* MyCharacter = Cast<ATFCharacter>(ControlledPawn);
// 		if (MyCharacter)
// 		{
// 			MyCharacter->Jump();
// 		}
// 	}
// }
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
