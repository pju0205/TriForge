// Fill out your copyright notice in the Description page of Project Settings.
#include "TFMyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "TFMyAnimInstance.h"
#include "CharacterTrajectoryComponent.h"

ATFMyCharacter::ATFMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	MovementComponent->bOrientRotationToMovement = false;
	MovementComponent->RotationRate = FRotator(0.f, 300.f, 0.f);

	CharacterTrajectory = CreateDefaultSubobject<UCharacterTrajectoryComponent>(TEXT("CharacterTrajectory"));
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->SetWorldRotation(FRotator(-30.0f, 0.0f, 0.0f));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);

	DefaultSpeed = 300.0f;
	SprintSpeed = 900.0f;
	MovementComponent->MaxWalkSpeed = DefaultSpeed;
}

void ATFMyCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ATFMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector Velocity = GetVelocity();
	float Speed = Velocity.Size();

	// 화면에 속도 출력
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,  // 고유 ID (-1이면 새 메시지 추가)
			0.0f, // 지속 시간 (0.0f로 설정하면 다음 틱에서 사라짐)
			FColor::Green, // 글씨 색상
			FString::Printf(TEXT("Speed: %.2f"), Speed) // 출력할 문자열
		);
	}
}

void ATFMyCharacter::UpdateSprintState(bool bIsSprint)
{
	IsSprinting = bIsSprint;

	if (IsSprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = DefaultSpeed; // 기본 속도 설정
	}
}

void ATFMyCharacter::UpdateCrouchState(bool bIsCrouch)
{
	if (GetCharacterMovement()->IsCrouching())
	{
		Crouch();
	}
	else
	{
		UnCrouch();
	}
}
