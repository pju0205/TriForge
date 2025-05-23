// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/TFRangedWeapon.h"

#include "Character/TFPlayerCharacter.h"
#include "Character/TFPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"



ATFRangedWeapon::ATFRangedWeapon()
{
	SetWeaponClass(EWeaponClass::Ewc_RangedWeapon);
}

void ATFRangedWeapon::TraceEnemy(FHitResult& TraceHitResult)
{
	if (TFOwnerCharacter == nullptr) return;
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
		FVector End = Start + CrosshairWorldDirection * TRACELENGTH;

		if (TFOwnerCharacter)
		{
			float DistanceToCharacter = (TFOwnerCharacter->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + 100.f);
			//DrawDebugSphere(GetWorld(), Start, 15.f, 12, FColor::Red, false);
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
		// End 값을 넘기지 않아서 아마 지워도 될듯?
		End = TraceHitResult.bBlockingHit ? TraceHitResult.ImpactPoint : End;
		//DrawDebugLine(GetWorld(), Start, End, FColor::Black, false, 3.f);
	}
}

void ATFRangedWeapon::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATFRangedWeapon, Ammo);
}

void ATFRangedWeapon::SetHUDRangedWeaponAmmo()
{
	TFOwnerCharacter = TFOwnerCharacter == nullptr ? Cast<ATFPlayerCharacter>(GetOwner()) : TFOwnerCharacter;
	if (TFOwnerCharacter)
	{
		TFOwnerController = TFOwnerController == nullptr ? Cast<ATFPlayerController>(TFOwnerCharacter->GetController()) : TFOwnerController;

		if (TFOwnerController)
		{
			TFOwnerController->SetHUDAmmo(Ammo);
		}
	}
}

bool ATFRangedWeapon::IsAmmoEmpty()
{
	return Ammo <= 0;
}

void ATFRangedWeapon::SpendAmmo()
{
	Ammo = FMath::Clamp(Ammo - 1 , 0, MagCapacity);

	SetHUDRangedWeaponAmmo();
}

void ATFRangedWeapon::OnRep_Ammo()
{
	SetHUDRangedWeaponAmmo();
}

void ATFRangedWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();
	if (Owner == nullptr)
	{
		TFOwnerCharacter = nullptr;
		TFOwnerController = nullptr;
	}
	else
	{
		SetHUDRangedWeaponAmmo();
	}

}

void ATFRangedWeapon::Dropped()
{
	TFOwnerCharacter = nullptr;
	TFOwnerController = nullptr;

	Super::Dropped();
}

void ATFRangedWeapon::MultiAttackEffects_Implementation()
{
	PlayAttackMontage();
	GetWeaponMesh()->PlayAnimation(RangedWeaponAnimation, false);
}
