// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TFPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "KismetAnimationLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Curves/CurveFloat.h"
#include "Net/UnrealNetwork.h"
#include "PlayerState/TFPlayerState.h"
#include "TriForge/TriForge.h"
#include "Weapon/TFWeaponComponent.h"

ATFPlayerCharacter::ATFPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	MovementComponent->bOrientRotationToMovement = false;
	MovementComponent->RotationRate = FRotator(0.f, 300.f, 0.f);

	// 1인칭 카메라 설정
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetMesh(), FName("head"));
	Camera->bUsePawnControlRotation = true;

	// Weapon
	WeaponComponent = CreateDefaultSubobject<UTFWeaponComponent>(TEXT("WeaponComponent"));
	WeaponComponent->SetIsReplicated(true);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	
	WalkSpeed = FVector(400.0f, 375.0f, 350.0f);			// default : 300 275 250
	SprintSpeed = FVector(1000.0f, 775.0f, 750.0f);			// default : 700 575 550
	ECurrentGait = E_Gait::Walk;
	bSprinting = false;
	bWalking = true;
	bJustLanded = false;
	LandVelocity = FVector(0.0f, 0.0f, 0.0f);
	SlideMontage = nullptr;
}

void ATFPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateMovement();
}

void ATFPlayerCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATFPlayerCharacter, bSprinting);
	DOREPLIFETIME(ATFPlayerCharacter, bWalking);

	DOREPLIFETIME_CONDITION(ATFPlayerCharacter, OverlappingWeapon, COND_OwnerOnly);
}


void ATFPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ThisClass::ReceiveDamage);
	}
}

void ATFPlayerCharacter::GetDesiredGait()
{
	if (bSprinting)
	{
		ECurrentGait = E_Gait::Sprint;
	}
	else
	{
		if (bWalking)
		{
			ECurrentGait = E_Gait::Walk;
		}
	}
}

float ATFPlayerCharacter::CalculateMaxSpeed(float& StrafeSpeedMap)
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	float MovementDirection = UKismetAnimationLibrary::CalculateDirection(MovementComponent->Velocity, GetActorRotation());
	MovementDirection = FMath::Abs(MovementDirection);
	if (StrafeSpeedMapCurve)
	{
		StrafeSpeedMap = StrafeSpeedMapCurve->GetFloatValue(MovementDirection);
	}

	FVector CurrentSpeed = FVector::ZeroVector;
	if (ECurrentGait == E_Gait::Walk)
	{
		CurrentSpeed = WalkSpeed;
	}
	else if (ECurrentGait == E_Gait::Sprint)
	{
		CurrentSpeed = SprintSpeed;
	}
	float FrontSpeed = UKismetMathLibrary::MapRangeClamped(StrafeSpeedMap, 0.0f, 1.0f, CurrentSpeed.X, CurrentSpeed.Y);
	float BackSpeed = UKismetMathLibrary::MapRangeClamped(StrafeSpeedMap, 1.0f, 2.0f, CurrentSpeed.Y, CurrentSpeed.Z);
	float FinalSpeed = UKismetMathLibrary::SelectFloat(FrontSpeed, BackSpeed, StrafeSpeedMap < 1.0);

	return FinalSpeed;
}

void ATFPlayerCharacter::UpdateMovement()
{
	float StrafeSpeedMap = 0.0f;
	
	GetDesiredGait();
	float CurrentSpeed = CalculateMaxSpeed(StrafeSpeedMap);

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	MovementComponent->MaxWalkSpeed = CurrentSpeed;
}

void ATFPlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();

	LandVelocity = MovementComponent->Velocity;

	bJustLanded = true;

	FLatentActionInfo LatentActionInfo;
	LatentActionInfo.CallbackTarget = this;
	LatentActionInfo.ExecutionFunction = FName("OnDelayComplete");
	LatentActionInfo.Linkage = 0;
	LatentActionInfo.UUID = __LINE__; // 각 지연 액션마다 고유 ID 필요
	
	UKismetSystemLibrary::RetriggerableDelay(this, 0.3f, LatentActionInfo);

}

void ATFPlayerCharacter::OnDelayComplete()
{
	// 0.3초 후에 Just Landed를 false로 설정
	bJustLanded = false;
}

void ATFPlayerCharacter::UpdateSprintState(bool bSprint)
{
	if (IsLocallyControlled())
	{
		ServerUpdateSprintState(bSprint);
	}
}

void ATFPlayerCharacter::ServerUpdateSprintState_Implementation(bool bSprint)
{
	bSprinting = bSprint;
	bWalking = !bSprint;
}

void ATFPlayerCharacter::isPlayingSlideMontage(float Forward, float Right)
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	float Velocity = UKismetMathLibrary::VSize(MovementComponent->Velocity);
	
	if (Velocity > 1.0f)
	{
		SetSlideDir(Forward, Right);
		PlaySlidMontage();
	}
}

void ATFPlayerCharacter::PlaySlidMontage()
{
	if (SlideMontage)
	{
		PlayAnimMontage(SlideMontage);
	}
}

void ATFPlayerCharacter::SetSlideDir(float Forward, float Right)
{
	SlideMontage = nullptr;

	if (Forward == 0)
	{
		if (Right)
		{
			if (Right > 0)
			{
				SlideMontage = RightSlide_Montage;
				if (GEngine)
					GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Slide Right"));
			}
			else
			{
				SlideMontage = LeftSlide_Montage;
				if (GEngine)
					GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Slide Left"));
			}
		}
	}
	else
	{
		if (Forward > 0)
		{
			SlideMontage = ForwardSlide_Montage;
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Slide Forward"));
		}
		else
		{
			SlideMontage = BackSlide_Montage;
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Slide Backward"));
		}
	}
}


void ATFPlayerCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
	class AController* InstigatedBy, AActor* DamageCauser)
{
	ATFPlayerCharacter* DamagedCharacter = Cast<ATFPlayerCharacter>(DamagedActor);
	if (DamagedCharacter)
	{
		TFPlayerState = TFPlayerState == nullptr ? Cast<ATFPlayerState>(DamagedCharacter->GetPlayerState()) : TFPlayerState;

		if (TFPlayerState)
		{
			TFPlayerState->CalcDamage(Damage);
		}
	}
}

void ATFPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (WeaponComponent)
	{
		WeaponComponent->PlayerCharacter = this;
	}
}

void ATFPlayerCharacter::SetOverlappingWeapon(ATFWeapon* Weapon)
{
	OverlappingWeapon = Weapon;
}

bool ATFPlayerCharacter::IsWeaponEquipped()
{
	return (WeaponComponent && WeaponComponent->EquippedWeapon);
}

bool ATFPlayerCharacter::IsAiming()
{
	return (WeaponComponent && WeaponComponent->bAiming);
}

void ATFPlayerCharacter::AimButtonPressed()
{
	if (WeaponComponent)
	{
		WeaponComponent->SetAiming(true);
	}
}

void ATFPlayerCharacter::AimButtonReleased()
{
	if (WeaponComponent)
	{
		WeaponComponent->SetAiming(false);
	}
}

void ATFPlayerCharacter::EquipButtonPressed()
{
	if (WeaponComponent)
	{
		ServerEquipButtonPressed();
	}
}

void ATFPlayerCharacter::ServerEquipButtonPressed_Implementation()
{
	if (WeaponComponent)
	{
		MulticastEquipButtonPressed();
	}
}

void ATFPlayerCharacter::MulticastEquipButtonPressed_Implementation()
{
	if (WeaponComponent)
	{
		WeaponComponent->EquipWeapon(OverlappingWeapon);
	}
}


void ATFPlayerCharacter::AttackButtonPressed()
{
	if (WeaponComponent)
	{
		WeaponComponent->AttackButtonPressed(true);
	}
}

void ATFPlayerCharacter::AttackButtonReleased()
{
	if (WeaponComponent)
	{
		WeaponComponent->AttackButtonPressed(false);
	}
}

UTFWeaponComponent* ATFPlayerCharacter::GetWeaponComponent()
{
	return WeaponComponent;
}

ATFWeapon* ATFPlayerCharacter::GetEquippedWeapon()
{
	if (WeaponComponent == nullptr) return nullptr;
	return WeaponComponent->EquippedWeapon;
}

void ATFPlayerCharacter::OnRep_OverlappingWeapon(ATFWeapon* LastWeapon)
{
	
}


