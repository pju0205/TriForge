// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/TFRifle.h"

ATFRifle::ATFRifle()
{
	SetWeaponType(EWeaponType::Ewt_Rifle);
	bAutomatic = true;
	AttackDelay = .15f;
}
