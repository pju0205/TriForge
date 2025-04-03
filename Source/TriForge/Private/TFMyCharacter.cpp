// Fill out your copyright notice in the Description page of Project Settings.
#include "TFMyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "TFMyAnimInstance.h"
#include "CharacterTrajectoryComponent.h"


// WeaponInclude
#include "Net/UnrealNetwork.h"
#include "Weapon/TFWeaponComponent.h"

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

	/*Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);*/

	DefaultSpeed = 300.0f;
	SprintSpeed = 900.0f;
	MovementComponent->MaxWalkSpeed = DefaultSpeed;

	// Weapon �ڵ� ����
	WeaponComponent = CreateDefaultSubobject<UTFWeaponComponent>(TEXT("WeaponComponent"));
	WeaponComponent->SetIsReplicated(true);
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

	// ȭ�鿡 �ӵ� ���
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,  // ���� ID (-1�̸� �� �޽��� �߰�)
			0.0f, // ���� �ð� (0.0f�� �����ϸ� ���� ƽ���� �����)
			FColor::Green, // �۾� ����
			FString::Printf(TEXT("Speed: %.2f"), Speed) // ����� ���ڿ�
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
		GetCharacterMovement()->MaxWalkSpeed = DefaultSpeed; // �⺻ �ӵ� ����
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

//
// Weapon �ڵ� ����
//

void ATFMyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (WeaponComponent)
	{
		WeaponComponent -> Character = this;
	}
}

void ATFMyCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ATFMyCharacter, OverlappingWeapon, COND_OwnerOnly);
}

void ATFMyCharacter::SetOverlappingWeapon(ATFWeapon* Weapon)
{
	OverlappingWeapon = Weapon;
}

void ATFMyCharacter::OnRep_OverlappingWeapon(ATFWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		
	}
}

bool ATFMyCharacter::bIsWeaponEquipped()
{
	return (WeaponComponent && WeaponComponent->EquippedWeapon);
}

void ATFMyCharacter::EquipButtonPressed()
{
	if (WeaponComponent)
	{
		ServerEquipButtonPressed();
	}
}

void ATFMyCharacter::ServerEquipButtonPressed_Implementation()
{
	if (WeaponComponent)
	{
		MulticastEquipButtonPressed();
	}
}

void ATFMyCharacter::MulticastEquipButtonPressed_Implementation()
{
	if (WeaponComponent)
	{
		WeaponComponent->EquipWeapon(OverlappingWeapon);
	}
}



