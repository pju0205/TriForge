// Fill out your copyright notice in the Description page of Project Settings.
#include "Character/TFCharacter.h"

ATFCharacter::ATFCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATFCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ATFCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
}
