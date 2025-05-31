

#include "Weapon/TFWeaponComponent.h"

#include "Engine/SkeletalMeshSocket.h"
#include "HUD/TFHUD.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "Camera/CameraComponent.h"
#include "Character/TFPlayerCharacter.h"
#include "Character/TFPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
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

	if (PlayerCharacter && PlayerCharacter->GetCamera())
	{
		DefaultFOV = PlayerCharacter->GetCamera()->FieldOfView;
		CurrentFOV = DefaultFOV;
	}
	
	ZERO_INIT(float, RecoilOffset);
}

void UTFWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
	if (PlayerCharacter && PlayerCharacter->IsLocallyControlled())
	{
		SetHUDCrosshairs(DeltaTime);
		InterpFOV(DeltaTime);
	}
	
	// 연사 가능 무기 (bAutomatic = true)에만 recoil 적용 
	if (bAttackButtonPressed && EquippedWeapon && EquippedWeapon->bAutomatic && CanAttack())
	{
		ApplyRecoil(DeltaTime);
	}
	else if (!bAttackButtonPressed && RecoilOffset.Size() > 0)
	{
		RecoilOffset.X = FMath::FInterpTo(RecoilOffset.X, 0.f, DeltaTime, 10.f);
		RecoilOffset.Y = FMath::FInterpTo(RecoilOffset.Y, 0.f, DeltaTime, 10.f);

		RecoilYawBias = FMath::FInterpTo(RecoilYawBias, 0.f, DeltaTime, 10.f);
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
	if (PlayerCharacter == nullptr || EquippedWeapon == nullptr) return;
	bAiming = bIsAiming;
	ServerSetAiming(bIsAiming);
	if (PlayerCharacter->IsLocallyControlled() && EquippedWeapon->GetWeaponType() == EWeaponType::Ewt_SniperRifle)
	{
		PlayerCharacter->ShowSniperScope(bIsAiming);
	}
}

void UTFWeaponComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;
}

void UTFWeaponComponent::InterpFOV(float DeltaTime)
{
	if (EquippedWeapon == nullptr) return;

	if (bAiming)
	{
		// 무기에 따라 줌되는 속도가 다름
		CurrentFOV = FMath::FInterpTo(
			CurrentFOV,
			EquippedWeapon->GetZoomedFOV(),
			DeltaTime, EquippedWeapon->GetZoomInterpSpeed()
		);
	}
	else
	{
		// 무기에 따라 줌 아웃되는 속도는 같음
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, ZoomInterpSpeed);
	}
	if (PlayerCharacter && PlayerCharacter->GetCamera())
	{
		PlayerCharacter->GetCamera()->SetFieldOfView(CurrentFOV);
	}
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
		CurrentFOV = DefaultFOV;
		SetAiming(false);
		
		/*if (PlayerCharacter->IsLocallyControlled() && EquippedWeapon->GetWeaponType() == EWeaponType::Ewt_SniperRifle)
		{
			PlayerCharacter->ShowSniperScope(false);
		}*/
		
		if (PlayerCharacter && PlayerCharacter->GetCamera())
		{
			PlayerCharacter->GetCamera()->SetFieldOfView(DefaultFOV);
		}
		
		if (PlayerController)
		{
			PlayerController->SetHUDAmmo(0);
		}
		
		EquippedWeapon = nullptr;
	}

	if (WeaponToEquip == nullptr) return;
	
	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::Ews_Equipped);

	const USkeletalMeshSocket* WeaponSocket = PlayerCharacter->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (WeaponSocket)
	{
		WeaponSocket->AttachActor(EquippedWeapon, PlayerCharacter->GetMesh());
		
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

void UTFWeaponComponent::ApplyRecoil(float DeltaTime)
{
	RecoilOffset.X = FMath::Clamp(RecoilOffset.X + FMath::FRandRange(-0.2,-0.1), -2.f, 0.f);
	PlayerController->AddPitchInput(RecoilOffset.X * DeltaTime); // 위아래

	// 단순히 Random으로 하면 좌우 반동 보다 한쪽으로 치우치는 경우가 많음
	// 편향값을 두어 한쪽으로 치우치면 반대값이 나오게끔 하였다.
	float YawRandom = FMath::FRandRange(-3.f, 3.f);
	float BiasStrength = 0.5f;  // 편향 적용 강도 0 ~ 1
	float BalancedYaw = YawRandom - RecoilYawBias * BiasStrength;
	RecoilOffset.Y += BalancedYaw;
	RecoilYawBias += BalancedYaw;
	
	PlayerController->AddYawInput(RecoilOffset.Y * DeltaTime); // 좌우
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

// 손에 들고 있는 무기 전부 삭제
void UTFWeaponComponent::DestroyWeapon()
{
	for (ATFWeapon* Weapon : Inventory)
	{
		if (IsValid(Weapon))
		{
			Weapon->Destroy();
		}
	}
}
