// Fill out your copyright notice in the Description page of Project Settings.


#include "TFMyAnimInstance.h"
#include "TFMyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CharacterTrajectoryComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "KismetAnimationLibrary.h"

UTFMyAnimInstance::UTFMyAnimInstance()
{
	/*Direction = 0;
	GroundSpeed = 0;
	IsJump = false;
	IsCrouching = false;*/
}

void UTFMyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	/*MyCharacter = Cast<ATFMyCharacter>(TryGetPawnOwner());
	if (MyCharacter != nullptr)
	{
		MyCharacterMovement = MyCharacter->GetCharacterMovement();
	}*/
}

void UTFMyAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
	/*if (MyCharacter)
	{
		CharacterTrajectory = MyCharacter->GetCharacterTrajectoryComponent();
		Direction = UKismetAnimationLibrary::CalculateDirection(MyCharacter->GetVelocity(), MyCharacter->GetActorRotation());
		GroundSpeed = UKismetMathLibrary::VSizeXY(MyCharacter->GetVelocity());
		IsJump = MyCharacterMovement->IsFalling();
		IsCrouching = MyCharacterMovement->IsCrouching();
	}*/
}
