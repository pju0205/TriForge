

#include "Weapon/TFWeaponComponent.h"

#include "Engine/SkeletalMeshSocket.h"
#include "HUD/TFHUD.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "Character/TFPlayerCharacter.h"
#include "Character/TFPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapon/TFMeleeWeapon.h"
#include "Weapon/TFRangedWeapon.h"
#include "Weapon/TFWeapon.h"

UTFWeaponComponent::UTFWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
}

void UTFWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void UTFWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
	if (PlayerCharacter && PlayerCharacter->IsLocallyControlled())
	{
		SetHUDCrosshairs(DeltaTime);
	}
	
}

void UTFWeaponComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTFWeaponComponent, EquippedWeapon);
	DOREPLIFETIME(UTFWeaponComponent, bAiming);
}

void UTFWeaponComponent::SetAiming(bool bIsAiming)
{
	bAiming = bIsAiming;
	ServerSetAiming(bIsAiming);
}

void UTFWeaponComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;
}

void UTFWeaponComponent::EquipWeapon(ATFWeapon* WeaponToEquip)
{
	if (PlayerCharacter == nullptr) return;
	// 무기를 들고 있고 WeaponToEquip != nullptr 이면 무기 교체
	// 무기를 들고 있고 WeaponToEquip == nullptr 이면 무기 드랍
	if (EquippedWeapon)
	{
		EquippedWeapon->Dropped();
		// TODO: 연속 발사 하는 도중에 총을 버리면 bCanFire가 false로 고정되어 총을 쏠 수 없게 되기에 true로 바꾸었다. 나중에 리팩토링이 필요할 수도 있다. 
		bCanAttack = true; 
		EquippedWeapon = nullptr;
		if (PlayerController)
		{
			PlayerController->SetHUDAmmo(0);
		}
	}

	if (WeaponToEquip == nullptr) return;
	
	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::Ews_Equipped);

	const USkeletalMeshSocket* WeaponSocket = PlayerCharacter->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (WeaponSocket)
	{
		WeaponSocket->AttachActor(EquippedWeapon, PlayerCharacter->GetMesh());

		/*WeaponSocket->RelativeLocation*/
		EquippedWeapon->SetActorRelativeLocation(EquippedWeapon->RightHandOffsetLocation);
		EquippedWeapon->SetActorRelativeRotation(EquippedWeapon->RightHandOffsetRotation);
	}
	
	EquippedWeapon->SetOwner(PlayerCharacter);

	// 만약 무기가 원거리 무기라면 HUD에 잔탄 수 보이게 하기
	if (EquippedWeapon->GetWeaponClass() == EWeaponClass::Ewc_RangedWeapon)
	{
		ATFRangedWeapon* RangedWeapon = Cast<ATFRangedWeapon>(EquippedWeapon);
		if (RangedWeapon)
		{
			RangedWeapon->SetHUDRangedWeaponAmmo();
		}
	}
	
	/*PlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	PlayerCharacter->bUseControllerRotationYaw = true;*/
}

void UTFWeaponComponent::OnRep_EquippedWeapon()
{
	if (EquippedWeapon && PlayerCharacter)
	{
		EquippedWeapon->SetWeaponState(EWeaponState::Ews_Equipped);

		const USkeletalMeshSocket* WeaponSocket = PlayerCharacter->GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (WeaponSocket)
		{
			WeaponSocket->AttachActor(EquippedWeapon, PlayerCharacter->GetMesh());
		}
		/*PlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		PlayerCharacter->bUseControllerRotationYaw = true;*/
	}
}

void UTFWeaponComponent::AttackButtonPressed(bool bPressed)
{
	bAttackButtonPressed = bPressed;
	
	if (EquippedWeapon == nullptr) return;
	if (bAttackButtonPressed && bCanAttack)
	{
		Attacking();
	}
	
}

void UTFWeaponComponent::Attacking()
{
	if (CanAttack())
	{
		bCanAttack = false;
		
		EquippedWeapon->Attack();
		
		StartAttackTimer();
	}
	
}

bool UTFWeaponComponent::CanAttack()
{
	if (EquippedWeapon == nullptr) return false;
	
	EWeaponClass EquippedWeaponClass = EquippedWeapon->GetWeaponClass();
	switch (EquippedWeaponClass)
	{
	case EWeaponClass::Ewc_RangedWeapon:
		{
			ATFRangedWeapon* RangedWeapon = Cast<ATFRangedWeapon>(EquippedWeapon);
			if (RangedWeapon)
			{
				return !RangedWeapon->IsAmmoEmpty() || !bCanAttack;
			}
			break;
		}
	case EWeaponClass::Ewc_MeleeWeapon:
		{
			ATFMeleeWeapon* MeleeWeapon = Cast<ATFMeleeWeapon>(EquippedWeapon);
			if (MeleeWeapon)
			{
				return !PlayerCharacter->GetMesh()->GetAnimInstance()->Montage_IsPlaying(EquippedWeapon->AttackMontage);
			}
			break;
		}
		default:
			break;
	}
	return bCanAttack;
}

void UTFWeaponComponent::StartAttackTimer()
{
	if (EquippedWeapon == nullptr || PlayerCharacter == nullptr) return;
	PlayerCharacter->GetWorldTimerManager().SetTimer(
		AttackTimer,
		this,
		&ThisClass::AttackTimerFinished,
		EquippedWeapon->AttackDelay
	);
}

void UTFWeaponComponent::AttackTimerFinished()
{
	if (EquippedWeapon == nullptr) return;
	bCanAttack = true;
	if (bAttackButtonPressed && EquippedWeapon->bAutomatic)
	{
		Attacking();
	}
}


ATFWeapon* UTFWeaponComponent::GetEquippedWeapon()
{
	return EquippedWeapon;
}

void UTFWeaponComponent::SetHUDCrosshairs(float DeltaTime)
{
	if (PlayerCharacter == nullptr || PlayerCharacter->Controller == nullptr) return;

	PlayerController = PlayerController == nullptr ? Cast<ATFPlayerController>(PlayerCharacter->Controller) : PlayerController;
	if (PlayerController)
	{
		HUD = HUD == nullptr ? Cast<ATFHUD>(PlayerController->GetHUD()) : HUD;

		if (HUD)
		{
			FHUDPackage HUDPackage;
			if (EquippedWeapon)
			{
				HUDPackage.CrosshairsCenter = EquippedWeapon->CrosshairsCenter;
				HUDPackage.CrosshairsLeft = EquippedWeapon->CrosshairsLeft;
				HUDPackage.CrosshairsRight = EquippedWeapon->CrosshairsRight;
				HUDPackage.CrosshairsTop = EquippedWeapon->CrosshairsTop;
				HUDPackage.CrosshairsBottom = EquippedWeapon->CrosshairsBottom;
			}
			else
			{
				HUDPackage.CrosshairsCenter = nullptr;
				HUDPackage.CrosshairsLeft = nullptr;
				HUDPackage.CrosshairsRight = nullptr;
				HUDPackage.CrosshairsTop = nullptr;
				HUDPackage.CrosshairsBottom = nullptr;
			}
			HUD->SetHUDPackage(HUDPackage);
		}
	}
}
