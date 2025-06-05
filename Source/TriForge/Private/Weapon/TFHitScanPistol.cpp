
#include "Weapon/TFHitScanPistol.h"

ATFHitScanPistol::ATFHitScanPistol()
{
	SetWeaponType(EWeaponType::Ewt_Pistol);
	Damage = 8.f;
	AttackDelay = 0.4f;
}
