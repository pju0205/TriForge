

#include "Weapon/TFWeaponComponent.h"

#include "Character/TFWeaponCharacter.h"
#include "Components/SphereComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
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

	FHitResult HitResult;
	TraceEnemy(HitResult);
}

void UTFWeaponComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTFWeaponComponent, EquippedWeapon);
}

void UTFWeaponComponent::EquipWeapon(ATFWeapon* WeaponToEquip)
{
	// Character == nullptr OR OverlappingWeapon == nullptr
	if (Character == nullptr || WeaponToEquip == nullptr) return;
	
	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::Ews_Equipped);

	const USkeletalMeshSocket* WeaponSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (WeaponSocket)
	{
		WeaponSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}
	
	EquippedWeapon->SetOwner(Character);
}

void UTFWeaponComponent::AttackButtonPressed(bool bPressed)
{
	bAttackButtonPressed = bPressed;
	
	if (EquippedWeapon == nullptr) return;
	if (bAttackButtonPressed)
	{
		ServerAttackButton();
	}
	
	
}

void UTFWeaponComponent::ServerAttackButton_Implementation()
{
	MulticastAttackButton();
}


void UTFWeaponComponent::MulticastAttackButton_Implementation()
{
	EquippedWeapon->PlayAttackMontage();
	EquippedWeapon->Attack(HitTarget);
	
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
		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECC_Visibility
		);
		if (!TraceHitResult.bBlockingHit)
		{
			// 만약 Linetrace를 통해 부딪힌 물체가 없다면 ImpactPoint를 끝지점으로 지정
			TraceHitResult.ImpactPoint = End;
			HitTarget = End;
		}
		else
		{
			HitTarget = TraceHitResult.ImpactPoint;
			DrawDebugSphere(
				GetWorld(),
				TraceHitResult.ImpactPoint,
				15.f,
				15,
				FColor::Red
			);
		}
	}
}
