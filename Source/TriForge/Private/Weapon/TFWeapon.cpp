

#include "Weapon/TFWeapon.h"

#include "Character/TFPlayerCharacter.h"
#include "Character/TFPlayerController.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundCue.h"
#include "Weapon/TFWeaponComponent.h"

ATFWeapon::ATFWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	//WeaponMesh->SetIsReplicated(true);
	WeaponMesh->SetCollisionResponseToAllChannels(ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 무기와 캐릭터가 일정거리로 좁혀졌는지 확인하기 위한 Sphere
	WeaponSphere = CreateDefaultSubobject<USphereComponent>(TEXT("WeaponSphere"));
	WeaponSphere->SetupAttachment(RootComponent);

	WeaponSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	WeaponSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);
	
}

void ATFWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		WeaponSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

		WeaponSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::ATFWeapon::SphereBeginOverlap);
		WeaponSphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::ATFWeapon::SphereEndOverlap);
	}
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}
}

void ATFWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATFWeapon::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATFWeapon, WeaponState);
}

void ATFWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State;
	
	switch (WeaponState)
	{
	case EWeaponState::Ews_Equipped:
		WeaponSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ShowPickupWidget(false);
		PlayEquipSound();
		break;
	case EWeaponState::Ews_Dropped:
		if (HasAuthority())
		{
			WeaponSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		PlayDropSound();
		break;
	}
}

void ATFWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::Ews_Equipped:
		WeaponSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EWeaponState::Ews_Dropped:
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	}
}


void ATFWeapon::SphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ATFPlayerCharacter* TFCharacter = Cast<ATFPlayerCharacter>(OtherActor);
	if (TFCharacter && PickupWidget)
	{
		// WeaponSphere에 캐릭터가 Overlap이면 OveralppingWeapon 값을 Overlap되어있는 무기로 하여
		// 현재 WeaponSphere에 캐릭터가 Overlap 되어있음을 알 수 있음
		TFCharacter->SetOverlappingWeapon(this);
		PickupWidget->SetVisibility(true);
	}
}

void ATFWeapon::SphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ATFPlayerCharacter* TFCharacter = Cast<ATFPlayerCharacter>(OtherActor);
	if (TFCharacter && PickupWidget)
	{
		// Sphere 범위 밖으로 나가면 Overlapping Weapon = nullptr로 해줌으로써
		// Overlap 되어있는 Weapon이 없다는 것을 의미
		TFCharacter->SetOverlappingWeapon(nullptr);
		PickupWidget->SetVisibility(false);
	}
}

void ATFWeapon::PlayAttackMontage()
{
	if (AttackMontage)
	{
		ATFPlayerCharacter* OwnerCharacter = Cast<ATFPlayerCharacter>(GetOwner());
		if (OwnerCharacter)
		{
			if (OwnerCharacter->GetWeaponComponent() == nullptr || OwnerCharacter->GetWeaponComponent()->GetEquippedWeapon() == nullptr)
			{
				return;
			}
			
			UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
			if (AnimInstance && AttackMontage)
			{
				AnimInstance->Montage_Play(AttackMontage);
			}
		}
	}
}

void ATFWeapon::Dropped()
{
	SetWeaponState(EWeaponState::Ews_Dropped);
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	WeaponMesh->DetachFromComponent(DetachRules);
	SetOwner(nullptr);
}

void ATFWeapon::Attack()
{
	/*if (RangedWeaponAnimation)
	{
		WeaponMesh->PlayAnimation(RangedWeaponAnimation, false);
	}*/
}

void ATFWeapon::PlayEquipSound()
{
	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			EquipSound,
			GetActorLocation()
		);
	}
}

void ATFWeapon::PlayDropSound()
{
	if (DropSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			DropSound,
			GetActorLocation()
		);
	}
}

void ATFWeapon::ShowPickupWidget(bool bShow)
{
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(bShow);
	}
}

void ATFWeapon::ShowWeaponIcon()
{
	ATFPlayerCharacter* TFPlayerCharacter = Cast<ATFPlayerCharacter>(GetOwner());
	if (TFPlayerCharacter)
	{
		ATFPlayerController* TFPlayerController = Cast<ATFPlayerController>(TFPlayerCharacter->GetController());
		if (TFPlayerController && WeaponIcon)
		{
			TFPlayerController->SetHUDWeaponImage(WeaponIcon);
		}
		
	}
}



