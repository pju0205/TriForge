// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TFWeaponAnimInstance.h"

#include "Character/TFWeaponCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UTFWeaponAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	TFCharacter = Cast<ATFWeaponCharacter>(TryGetPawnOwner());
}

void UTFWeaponAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (TFCharacter == nullptr)
	{
		TFCharacter = Cast<ATFWeaponCharacter>(TryGetPawnOwner());
	}
	if (TFCharacter == nullptr) return;

	FVector Velocity = TFCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bIsInAir = TFCharacter->GetCharacterMovement()->IsFalling();

	bIsSprint = TFCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0 ? true : false;
}
