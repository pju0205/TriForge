

#include "Character/TFWeaponCharacter.h"

#include "Camera/CameraComponent.h"
#include "Character/TFWeaponPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "PlayerState/TFPlayerState.h"
#include "Weapon/TFWeaponComponent.h"

ATFWeaponCharacter::ATFWeaponCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	WeaponComponent = CreateDefaultSubobject<UTFWeaponComponent>(TEXT("WeaponComponent"));
	WeaponComponent->SetIsReplicated(true);
}

void ATFWeaponCharacter::BeginPlay()
{
	Super::BeginPlay();

	/*TFPlayerController = TFPlayerController == nullptr ? Cast<ATFWeaponPlayerController>(Controller) : TFPlayerController;
	TFPlayerState = TFPlayerState == nullptr ? Cast<ATFPlayerState>(GetPlayerState()) : TFPlayerState;*/
	
	/*if (TFPlayerController && TFPlayerState)
	{
		TFPlayerController->SetHUDHealth(TFPlayerState->CurrentHealth, TFPlayerState->MaxHealth);
	}
	else if (!TFPlayerState)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,15.f, FColor::Red, FString(TEXT("NoState")));
		}
	}
	else if (!TFPlayerController)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Black, FString(TEXT("NoController")));
		}
	}*/
	
	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ThisClass::ReceiveDamage);
	}
	
}

void ATFWeaponCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
	class AController* InstigatedBy, AActor* DamageCauser)
{
	TFPlayerState = TFPlayerState == nullptr ? Cast<ATFPlayerState>(GetPlayerState()) : TFPlayerState;
	if (TFPlayerState)
	{
		TFPlayerState->CalcDamage(Damage);
	}
}

void ATFWeaponCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATFWeaponCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ATFWeaponCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (WeaponComponent)
	{
		WeaponComponent->Character = this;
	}
}

void ATFWeaponCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ATFWeaponCharacter, OverlappingWeapon, COND_OwnerOnly);
}

void ATFWeaponCharacter::OnRep_OverlappingWeapon(ATFWeapon* LastWeapon)
{
	
}

void ATFWeaponCharacter::SetOverlappingWeapon(ATFWeapon* Weapon)
{
	OverlappingWeapon = Weapon;
}

bool ATFWeaponCharacter::IsWeaponEquipped()
{
	return (WeaponComponent && WeaponComponent->EquippedWeapon);
}

void ATFWeaponCharacter::EquipButtonPressed()
{
	if (WeaponComponent)
	{
		ServerEquipButtonPressed();
	}
}


void ATFWeaponCharacter::ServerEquipButtonPressed_Implementation()
{
	if (WeaponComponent)
	{
		MulticastEquipButtonPressed();
	}
	// 장착하는 기능에 대해서는 NetMulticast를 하지 않아도 된다.
	// 이 코드에서 multicast를 사용하지 않으면 네트워크 트래픽이 줄어드는 장점이 있다.
	// 하지만 즉각적인 재생이 필요한 애니메이션이나 피격효과, 사운드 등에서는 NetMulticast가 필요하다.
}

void ATFWeaponCharacter::MulticastEquipButtonPressed_Implementation()
{
	if (WeaponComponent)
	{
		WeaponComponent->EquipWeapon(OverlappingWeapon);
	}
}

void ATFWeaponCharacter::AttackButtonPressed()
{
	if (WeaponComponent)
	{
		WeaponComponent->AttackButtonPressed(true);
	}
}

void ATFWeaponCharacter::AttackButtonReleased()
{
	if (WeaponComponent)
	{
		WeaponComponent->AttackButtonPressed(false);
	}
}

UTFWeaponComponent* ATFWeaponCharacter::GetWeaponComponent()
{
	return WeaponComponent;
}

FVector ATFWeaponCharacter::GetHitTarget() const
{
	if (WeaponComponent == nullptr) return FVector();
	
	return WeaponComponent->HitTarget;
}


