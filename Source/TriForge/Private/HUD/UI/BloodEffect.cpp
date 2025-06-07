// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/UI/BloodEffect.h"

void UBloodEffect::PlayDamageAnimation()
{
	if (DamageFlashAnim)
	{
		PlayAnimation(DamageFlashAnim);
	}
}
