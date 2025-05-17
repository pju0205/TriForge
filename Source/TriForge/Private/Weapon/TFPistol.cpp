// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/TFPistol.h"

ATFPistol::ATFPistol()
{
	SetWeaponType(EWeaponType::EWt_Pistol);
	bAutomatic = false;
	AttackDelay = .8f;
}
