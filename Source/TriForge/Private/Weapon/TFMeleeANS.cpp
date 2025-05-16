

#include "Weapon/TFMeleeANS.h"

#include "Character/TFPlayerCharacter.h"
#include "Weapon/TFMeleeWeapon.h"

void UTFMeleeANS::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	ATFPlayerCharacter* PlayerCharacter = Cast<ATFPlayerCharacter>(MeshComp->GetOwner());
	if (PlayerCharacter && PlayerCharacter->GetEquippedWeapon())
	{
		ATFMeleeWeapon* MeleeWeapon = Cast<ATFMeleeWeapon>(PlayerCharacter->GetEquippedWeapon());
		if (MeleeWeapon)
		{
			MeleeWeapon->BeginTrace();
		}
	}
}

void UTFMeleeANS::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ATFPlayerCharacter* PlayerCharacter = Cast<ATFPlayerCharacter>(MeshComp->GetOwner());
	if (PlayerCharacter && PlayerCharacter->GetEquippedWeapon())
	{
		ATFMeleeWeapon* MeleeWeapon = Cast<ATFMeleeWeapon>(PlayerCharacter->GetEquippedWeapon());
		if (MeleeWeapon)
		{
			MeleeWeapon->EndTrace();
		}
	}
}
