// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TFPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "KismetAnimationLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Curves/CurveFloat.h"

ATFPlayerCharacter::ATFPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	MovementComponent->bOrientRotationToMovement = false;
	MovementComponent->RotationRate = FRotator(0.f, 300.f, 0.f);
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->SetWorldRotation(FRotator(-30.0f, 0.0f, 0.0f));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);

	WalkSpeed = FVector(300.0f, 275.0f, 250.0f);
	SprintSpeed = FVector(700.0f, 575.0f, 550.0f);
	ECurrentGait = E_Gait::Walk;
	bSprinting = false;
	bWalking = true;
	
}

void ATFPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateMovement();
}


void ATFPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ATFPlayerCharacter::GetDesiredGait()
{
	if (bSprinting)
	{
		ECurrentGait = E_Gait::Sprint;
	}
	else
	{
		if (bWalking)
		{
			ECurrentGait = E_Gait::Walk;
		}
	}
}

float ATFPlayerCharacter::CalculateMaxSpeed(float& StrafeSpeedMap)
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	float MovementDirection = UKismetAnimationLibrary::CalculateDirection(MovementComponent->Velocity, GetActorRotation());
	MovementDirection = FMath::Abs(MovementDirection);
	if (StrafeSpeedMapCurve)
	{
		StrafeSpeedMap = StrafeSpeedMapCurve->GetFloatValue(MovementDirection);
	}

	FVector CurrentSpeed = FVector::ZeroVector;
	if (ECurrentGait == E_Gait::Walk)
	{
		CurrentSpeed = WalkSpeed;
	}
	else if (ECurrentGait == E_Gait::Sprint)
	{
		CurrentSpeed = SprintSpeed;
	}
	float FrontSpeed = UKismetMathLibrary::MapRangeClamped(StrafeSpeedMap, 0.0f, 1.0f, CurrentSpeed.X, CurrentSpeed.Y);
	float BackSpeed = UKismetMathLibrary::MapRangeClamped(StrafeSpeedMap, 1.0f, 2.0f, CurrentSpeed.Y, CurrentSpeed.Z);
	float FinalSpeed = UKismetMathLibrary::SelectFloat(FrontSpeed, BackSpeed, StrafeSpeedMap < 1.0);

	return FinalSpeed;
}

void ATFPlayerCharacter::UpdateMovement()
{
	float StrafeSpeedMap = 0.0f;
	
	GetDesiredGait();
	float CurrentSpeed = CalculateMaxSpeed(StrafeSpeedMap);

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	MovementComponent->MaxWalkSpeed = CurrentSpeed;
}
