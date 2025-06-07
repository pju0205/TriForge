// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/TFProjectileBullet.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

void ATFProjectileBullet::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                FVector NormalImpulse, const FHitResult& Hit)
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter)
	{
		AController* OwnerController = OwnerCharacter->Controller;
		if (OwnerController)
		{
			const float CalculatedDamage = Hit.BoneName.ToString() == FString("head") ? HeadShotDamage : Damage;
			
			UGameplayStatics::ApplyDamage(OtherActor, CalculatedDamage, OwnerController, this, UDamageType::StaticClass());
			
		}
	}

	// 부모 클래스의 OnHit에서 Destroy가 호출되어있어 Super를 제일 마지막에 호출
	Super::OnHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
}
