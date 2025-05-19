// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TFEliminationComponent.generated.h"


class ATFMatchPlayerState;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TRIFORGE_API UTFEliminationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTFEliminationComponent();

protected:
	virtual void BeginPlay() override;

private:
	ATFMatchPlayerState* GetPlayerStateFromActor(AActor* Actor);
	void ProcessHitOrMiss(bool bHit, ATFMatchPlayerState* AttackerPS);
};
