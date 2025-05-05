

#include "Weapon/TFWeaponComponent.h"

#include "Character/TFWeaponCharacter.h"
#include "Character/TFWeaponPlayerController.h"
#include "Engine/SkeletalMeshSocket.h"
#include "HUD/TFHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
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

	
	if (Character && Character->IsLocallyControlled())
	{
		SetHUDCrosshairs(DeltaTime);
	}
	
}

void UTFWeaponComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTFWeaponComponent, EquippedWeapon);
}

void UTFWeaponComponent::EquipWeapon(ATFWeapon* WeaponToEquip)
{
	if (Character == nullptr) return;
	// 무기를 들고 있고 WeaponToEquip != nullptr 이면 무기 교체
	// 무기를 들고 있고 WeaponToEquip == nullptr 이면 무기 드랍
	if (EquippedWeapon)
	{
		EquippedWeapon->Dropped();
		// TODO: 연속 발사 하는 도중에 총을 버리면 bCanFire가 false로 고정되어 총을 쏠 수 없게 되기에 true로 바꾸었다. 나중에 리팩토링이 필요할 수도 있다. 
		bCanAttack = true; 
		EquippedWeapon = nullptr;
	}

	if (WeaponToEquip == nullptr) return;
	
	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::Ews_Equipped);

	const USkeletalMeshSocket* WeaponSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (WeaponSocket)
	{
		WeaponSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}
	
	EquippedWeapon->SetOwner(Character);

	// 만약 무기가 원거리 무기라면 HUD에 잔탄 수 보이게 하기
	if (EquippedWeapon->GetWeaponClass() == EWeaponClass::Ewc_RangedWeapon)
	{
		ATFRangedWeapon* RangedWeapon = Cast<ATFRangedWeapon>(EquippedWeapon);
		if (RangedWeapon)
		{
			RangedWeapon->SetHUDRangedWeaponAmmo();
		}
	}
	
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
}

void UTFWeaponComponent::OnRep_EquippedWeapon()
{
	if (EquippedWeapon && Character)
	{
		EquippedWeapon->SetWeaponState(EWeaponState::Ews_Equipped);

		const USkeletalMeshSocket* WeaponSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (WeaponSocket)
		{
			WeaponSocket->AttachActor(EquippedWeapon, Character->GetMesh());
		}
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
	}
}

void UTFWeaponComponent::AttackButtonPressed(bool bPressed)
{
	bAttackButtonPressed = bPressed;
	
	if (EquippedWeapon == nullptr) return;
	if (bAttackButtonPressed)
	{
		Attacking();
	}
	
}

void UTFWeaponComponent::Attacking()
{
	if (CanAttack())
	{
		bCanAttack = false;
		
		EWeaponClass EquippedWeaponClass = EquippedWeapon->GetWeaponClass();
		switch (EquippedWeaponClass)
		{
		case EWeaponClass::Ewc_RangedWeapon :
			FHitResult Result;
			TraceEnemy(Result);
			
			const USkeletalMeshSocket* MuzzleFlashSocket = EquippedWeapon->GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
			FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(EquippedWeapon->GetWeaponMesh());
			FVector SocketLocation = SocketTransform.GetLocation();
			
			EquippedWeapon->Attack(Result, SocketLocation);
			break;
		}
		
		StartAttackTimer();
	}
	
}

bool UTFWeaponComponent::CanAttack()
{
	if (EquippedWeapon == nullptr) return false;
	if (EquippedWeapon->GetWeaponClass() == EWeaponClass::Ewc_RangedWeapon)
	{
		ATFRangedWeapon* RangedWeapon = Cast<ATFRangedWeapon>(EquippedWeapon);
		if (RangedWeapon)
		{
			return !RangedWeapon->IsAmmoEmpty() || !bCanAttack;
		}
	}
	return bCanAttack;
}

void UTFWeaponComponent::StartAttackTimer()
{
	if (EquippedWeapon == nullptr || Character == nullptr) return;
	Character->GetWorldTimerManager().SetTimer(
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

void UTFWeaponComponent::TraceEnemy(FHitResult& TraceHitResult)
{
	// 화면 중앙 Trace

	// 화면의 중앙을 알려면 Viewport를 가져와야 한다.
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	// 화면 중앙을 조준점 위치로 지정
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

	//DeProjectScreenTowWorld 2D ScreenSpace를 3D WorldSpace로 transform 해주는 함수
	//게임 플레이어의 화면에서의 마우스 위치를 월드상에서의 좌표로 변환해준다.
	//우리가 조준하는 곳은 화면상에서의 2D 좌표이며 실질적으로 총알이 날라가는 위치는 월드상에서 3D 좌표이다.

	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);

	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;
		// 끝지점 = 시작 지점 + WorldDirection 방향으로 곱한 값만큼의 좌표 
		FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;

		if (Character)
		{
			float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + 100.f);
			DrawDebugSphere(GetWorld(), Start, 15.f, 12, FColor::Red, false);
		}
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(GetOwner());
		
		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECC_Visibility,
			Params
		);
		End = TraceHitResult.bBlockingHit ? TraceHitResult.ImpactPoint : End;
		//DrawDebugLine(GetWorld(), Start, End, FColor::Black, false, 3.f);
	}
	
}

void UTFWeaponComponent::SetHUDCrosshairs(float DeltaTime)
{
	if (Character == nullptr || Character->Controller == nullptr) return;

	Controller = Controller == nullptr ? Cast<ATFWeaponPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		HUD = HUD == nullptr ? Cast<ATFHUD>(Controller->GetHUD()) : HUD;

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
