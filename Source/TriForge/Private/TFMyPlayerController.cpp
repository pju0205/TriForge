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
		// 0은 우선순위(priority)를 의미. 우선순위는 여러 입력 매핑 컨텍스트가 있을 때, 어떤 컨텍스트가 우선 적용될지를 결정하는데 사용
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

		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y); // W/S 키
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);    // A/D 키
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
// Weapon 코드 시작 
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
