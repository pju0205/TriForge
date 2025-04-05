

#include "Character/TFWeaponCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
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

void ATFWeaponCharacter::SetOverlappingWeapon(ATFWeapon* Weapon)
{
	OverlappingWeapon = Weapon;
}

bool ATFWeaponCharacter::bIsWeaponEquipped()
{
	return (WeaponComponent && WeaponComponent->EquippedWeapon);
}

void ATFWeaponCharacter::EquipButtonPressed()
{
	if (WeaponComponent)
	{
		ServerEquipButtonPressed();
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("ServerEquip")));
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Black, FString(TEXT("There's no WeaponComp")));
		}
	}
}

void ATFWeaponCharacter::ServerEquipButtonPressed_Implementation()
{
	if (WeaponComponent)
	{
		MulticastEquipButtonPressed();
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("MuliEquip")));
		}
	}
}

void ATFWeaponCharacter::MulticastEquipButtonPressed_Implementation()
{
	if (WeaponComponent)
	{
		WeaponComponent->EquipWeapon(OverlappingWeapon);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("Equip")));
		}
	}
}


void ATFWeaponCharacter::OnRep_OverlappingWeapon(ATFWeapon* LastWeapon)
{
	
}

