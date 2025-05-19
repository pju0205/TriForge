// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/TFEliminationComponent.h"
#include "playerState/TFMatchPlayerState.h"


// Sets default values for this component's properties
UTFEliminationComponent::UTFEliminationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UTFEliminationComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

ATFMatchPlayerState* UTFEliminationComponent::GetPlayerStateFromActor(AActor* Actor)
{
	APawn* Pawn = Cast<APawn>(Actor);
	if (IsValid(Pawn))
	{
		return Pawn->GetPlayerState<ATFMatchPlayerState>();
	}
	return nullptr;
}

void UTFEliminationComponent::ProcessHitOrMiss(bool bHit, ATFMatchPlayerState* AttackerPS)
{
	if (bHit)
	{
		AttackerPS->AddHit();
	}
	else
	{
		AttackerPS->AddMiss();
	}
}
