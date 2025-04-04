// Fill out your copyright notice in the Description page of Project Settings.


#include "TFMyPlayerController.h"
#include "TFMyCharacter.h"
#include "TFMyAnimInstance.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

ATFMyPlayerController::ATFMyPlayerController()
{

}

void ATFMyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		// 0�� �켱����(priority)�� �ǹ�. �켱������ ���� �Է� ���� ���ؽ�Ʈ�� ���� ��, � ���ؽ�Ʈ�� �켱 ��������� �����ϴµ� ���
		Subsystem->AddMappingContext(MyCharacterContext, 0);
	}
}

void ATFMyPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATFMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	/*EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATFMyPlayerController::Move);
	EnhancedInputComponent->BindAction(RotationAction, ETriggerEvent::Triggered, this, &ATFMyPlayerController::Rotation);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ATFMyPlayerController::Jump);
	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ATFMyPlayerController::CrouchStart);
	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ATFMyPlayerController::CrouchEnd);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ATFMyPlayerController::SprintStart);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ATFMyPlayerController::SprintEnd);*/

	EnhancedInputComponent->BindAction(EquipActoin, ETriggerEvent::Started, this, &ThisClass::EquipWeapon);
	EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ThisClass::EquipWeapon);
	EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ThisClass::EquipWeapon);

}


/*void ATFMyPlayerController::Move(const struct FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{

		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y); // W/S Ű
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);    // A/D Ű
	}
}

void ATFMyPlayerController::Rotation(const FInputActionValue& InputActionValue)
{
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		const FVector2D RotationValue = InputActionValue.Get<FVector2D>();

		AddYawInput(RotationValue.X);
		AddPitchInput(RotationValue.Y);

	}
}

void ATFMyPlayerController::SprintStart(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ATFMyCharacter* MyCharacter = Cast<ATFMyCharacter>(ControlledPawn);
		if (MyCharacter)
		{
			MyCharacter->UpdateSprintState(true);
		}
	}
}

void ATFMyPlayerController::SprintEnd(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ATFMyCharacter* MyCharacter = Cast<ATFMyCharacter>(ControlledPawn);
		if (MyCharacter)
		{
			MyCharacter->UpdateSprintState(false);
		}
	}
}


void ATFMyPlayerController::Jump(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ATFMyCharacter* MyCharacter = Cast<ATFMyCharacter>(ControlledPawn);
		if (MyCharacter)
		{
			MyCharacter->Jump();
		}
	}
}


void ATFMyPlayerController::CrouchStart(const FInputActionValue& InputActionValue)
{

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ATFMyCharacter* MyCharacter = Cast<ATFMyCharacter>(ControlledPawn);
		if (MyCharacter)
		{
			MyCharacter->UpdateCrouchState(true);
		}
	}
}

void ATFMyPlayerController::CrouchEnd(const FInputActionValue& InputActionValue)
{

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ATFMyCharacter* MyCharacter = Cast<ATFMyCharacter>(ControlledPawn);
		if (MyCharacter)
		{
			MyCharacter->UpdateCrouchState(false);
		}
	}
}*/


//
// Weapon �ڵ� ���� 
//
void ATFMyPlayerController::EquipWeapon(const struct FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ATFMyCharacter* TFCharacter = Cast<ATFMyCharacter>(ControlledPawn);
		if (TFCharacter)
		{
			TFCharacter->EquipButtonPressed();
		}
	}
}


void ATFMyPlayerController::FireWeapon(const struct FInputActionValue& InputActionValue)
{
	
}
