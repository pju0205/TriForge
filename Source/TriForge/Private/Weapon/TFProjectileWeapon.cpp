// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/TFProjectileWeapon.h"

#include "Character/TFPlayerCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon/TFProjectile.h"

ATFProjectileWeapon::ATFProjectileWeapon()
{
	
}

void ATFProjectileWeapon::Attack()
{
	FHitResult Result;
	float Length = 8000.f;
	TraceEnemy(Result, Length);
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
	FVector SocketLocation = SocketTransform.GetLocation();

	ServerAttack(Result, SocketLocation);
}

void ATFProjectileWeapon::ServerAttack_Implementation(const FHitResult& HitResult, const FVector& SocketLocation)
{
	// 디버깅용 코드
	/*if (GEngine)
	{
		FVector aa = HitResult.ImpactPoint;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
			FString::Printf(TEXT("%f, %f, %f"), aa.X, aa.Y, aa.Z));
		GEngine->AddOnScreenDebugMessage(-1,5.f, FColor::Cyan,
			FString::Printf(TEXT("%f, %f, %f"),HitResult.TraceEnd.X, HitResult.TraceEnd.Y, HitResult.TraceEnd.Z));
	}
	DrawDebugSphere(GetWorld(), SocketLocation, 12.f, 5, FColor::Green, false);
	DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 12.f, 5, FColor::Blue, false);
	DrawDebugLine(GetWorld(), SocketLocation, HitResult.ImpactPoint, FColor::Red, false, 3.f);
	DrawDebugLine(GetWorld(), SocketLocation, HitResult.TraceEnd, FColor::Yellow, false, 3.f);*/

	//TODO: WeaponComponent => Attack() => 무기클래스의 Attack()함수 안에서 LineTrace => ServerAttack(HitResult&) 으로 구조 바꾸기
	//TODO: ServerAttack()에서 LineTrace값 받아서 SpawnActor로 Projectile 발사
	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	if (HitResult.bBlockingHit)
	{
		if (ProjectileClass && InstigatorPawn)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GetOwner();
			SpawnParams.Instigator = InstigatorPawn;
			UWorld* World = GetWorld();
			if (World)
			{
				World->SpawnActor<ATFProjectile>(
					ProjectileClass,
					SocketLocation,
					(HitResult.ImpactPoint-SocketLocation).Rotation(),
					SpawnParams
				);
			}
		}
	}
	else
	{
		if (ProjectileClass && InstigatorPawn)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GetOwner();
			SpawnParams.Instigator = InstigatorPawn;
			UWorld* World = GetWorld();
			if (World)
			{
				World->SpawnActor<ATFProjectile>(
					ProjectileClass,
					SocketLocation,
					(HitResult.TraceEnd - SocketLocation).Rotation(),
					SpawnParams
				);
			}
		}
	}
	AttackEffects();

	SpendAmmo();
	
}
void ATFProjectileWeapon::AttackEffects_Implementation()
{
	PlayAttackMontage();
	GetWeaponMesh()->PlayAnimation(RangedWeaponAnimation, false);
}



void ATFProjectileWeapon::TraceEnemy(FHitResult& TraceHitResult, float TraceLength)
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
		FVector End = Start + CrosshairWorldDirection * TraceLength;

		if (TFOwnerCharacter)
		{
			float DistanceToCharacter = (TFOwnerCharacter->GetActorLocation() - Start).Size();
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
		// End 값을 넘기지 않아서 아마 지워도 될듯?
		End = TraceHitResult.bBlockingHit ? TraceHitResult.ImpactPoint : End;
		//DrawDebugLine(GetWorld(), Start, End, FColor::Black, false, 3.f);
	}
}
