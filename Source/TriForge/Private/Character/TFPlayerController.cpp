// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TFPlayerController.h"
#include "Character/TFCharacter.h"
#include "Character/TFAnimInstance.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Character/Component/TFPlayerHealthComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/TFHUD.h"
#include "HUD/TFOverlay.h"
#include "HUD/UI/PlayerHealthBar.h"
#include "HUD/UI/RoundIndicator.h"
#include "PlayerState/TFMatchPlayerState.h"

ATFPlayerController::ATFPlayerController()
{
	bQuitMenuOpen = false;	// Quit 버튼 상태
	bPawnAlive = true;
}

void ATFPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->AddMappingContext(TFCharacterContext, 0);
	}
	
	OnPossessedPawnChanged.AddDynamic(this, &ATFPlayerController::HandlePawnChanged);
}

void ATFPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	bPawnAlive = true;
	TFHUD = Cast<ATFHUD>(GetHUD());

	if (HealthBar)
	{
		HealthBar->SetHealthBar();
	}
}

void ATFPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	OnPlayerStateReplicated.Broadcast();
}

void ATFPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

UTFPlayerHealthComponent* ATFPlayerController::GetHealthComponent() const
{
	APawn* MyPawn = GetPawn();
	if (IsValid(MyPawn))
	{
		return MyPawn->FindComponentByClass<UTFPlayerHealthComponent>();
	}
	return nullptr;
}

void ATFPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATFPlayerController::Move);
	EnhancedInputComponent->BindAction(RotationAction, ETriggerEvent::Triggered, this, &ATFPlayerController::Rotation);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ATFPlayerController::Jump);
	EnhancedInputComponent->BindAction(SlideAction, ETriggerEvent::Started, this, &ATFPlayerController::Slide);
	// EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ATFPlayerController::CrouchStart);
	// EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ATFPlayerController::CrouchEnd);
	// EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ATFPlayerController::SprintStart);
	// EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ATFPlayerController::SprintEnd);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ATFPlayerController::SprintStart);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ATFPlayerController::SprintEnd);
	EnhancedInputComponent->BindAction(QuitAction, ETriggerEvent::Started, this, &ATFPlayerController::Input_Quit);	// Quit 버튼


	EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ATFPlayerController::AimingStarted);	
	EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ATFPlayerController::AimingReleased);	
	EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &ATFPlayerController::EquipWeapon);
	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ATFPlayerController::WeaponAttackStarted);
	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &ATFPlayerController::WeaponAttackReleased);

}

void ATFPlayerController::Move(const struct FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{

		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);

		MoveDir = InputAxisVector;
	}
	
}

void ATFPlayerController::Rotation(const FInputActionValue& InputActionValue)
{
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		const FVector2D RotationValue = InputActionValue.Get<FVector2D>();

		ControlledPawn->AddControllerYawInput(RotationValue.X);
		ControlledPawn->AddControllerPitchInput(RotationValue.Y);
		// AddYawInput(RotationValue.X);
		// AddPitchInput(RotationValue.Y);

	}
}

void ATFPlayerController::SprintStart(const FInputActionValue& InputActionValue)
{
	UE_LOG(LogTemp, Warning, TEXT(">> SprintStart Triggered"));
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ATFPlayerCharacter* TFCharacter = Cast<ATFPlayerCharacter>(ControlledPawn);
		if (TFCharacter)
		{
			TFCharacter->UpdateSprintState(true);
		}
	}
}

void ATFPlayerController::SprintEnd(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ATFPlayerCharacter* TFCharacter = Cast<ATFPlayerCharacter>(ControlledPawn);
		if (TFCharacter)
		{
			TFCharacter->UpdateSprintState(false);
		}
	}
}

void ATFPlayerController::Jump(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ATFPlayerCharacter* TFCharacter = Cast<ATFPlayerCharacter>(ControlledPawn);
		if (TFCharacter)
		{
			TFCharacter->CustomJump();
		}
	}
}

void ATFPlayerController::Slide(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ATFPlayerCharacter* TFCharacter = Cast<ATFPlayerCharacter>(ControlledPawn);
		if (!TFCharacter) return;

		if (TFCharacter->GetGait() == E_Gait::Sprint && TFCharacter->GetIsSliding() == false &&  !TFCharacter->GetCharacterMovement()->IsFalling())
			// 현재 캐릭터가 달리는 중이고 && 슬라이딩 중이 아니고 && 공중이 아닐 때
		{
			FVector Velocity = TFCharacter->GetVelocity().GetSafeNormal2D();
			FVector Forward = TFCharacter->GetActorForwardVector().GetSafeNormal2D();

			float Dot = FVector::DotProduct(Forward, Velocity);

			// 앞을 보고 있을 때만 실행 
			if (Dot >= 0.8f)
			{
				if (GEngine)
					GEngine->AddOnScreenDebugMessage(10, 1,FColor::Red, "Controller Slid Call");
				TFCharacter->PlaySlidMontage();
			}
		}
	}
}

//
// void ATFPlayerController::CrouchStart(const FInputActionValue& InputActionValue)
// {
//
// 	if (APawn* ControlledPawn = GetPawn<APawn>())
// 	{
// 		ATFCharacter* MyCharacter = Cast<ATFCharacter>(ControlledPawn);
// 		if (MyCharacter)
// 		{
// 			MyCharacter->UpdateCrouchState(true);
// 		}
// 	}
// }
//
// void ATFPlayerController::CrouchEnd(const FInputActionValue& InputActionValue)
// {
//
// 	if (APawn* ControlledPawn = GetPawn<APawn>())
// 	{
// 		ATFCharacter* MyCharacter = Cast<ATFCharacter>(ControlledPawn);
// 		if (MyCharacter)
// 		{
// 			MyCharacter->UpdateCrouchState(false);
// 		}
// 	}
// }

void ATFPlayerController::Input_Quit()
{
	bQuitMenuOpen = !bQuitMenuOpen;
	if (bQuitMenuOpen)
	{
		FInputModeGameAndUI InputMode;
		SetInputMode(InputMode);
		SetShowMouseCursor(true);
		OnQuitMenuOpen.Broadcast(true);
	}
	else
	{
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
		SetShowMouseCursor(false);
		OnQuitMenuOpen.Broadcast(false);
	}
}

void ATFPlayerController::AimingStarted(const struct FInputActionValue& AimActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ATFPlayerCharacter* TFCharacter = Cast<ATFPlayerCharacter>(ControlledPawn);
		if (TFCharacter)
		{
			TFCharacter->AimButtonPressed();
		}
	}
}

void ATFPlayerController::AimingReleased(const struct FInputActionValue& AimActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ATFPlayerCharacter* TFCharacter = Cast<ATFPlayerCharacter>(ControlledPawn);
		if (TFCharacter)
		{
			TFCharacter->AimButtonReleased();
		}
	}
}

void ATFPlayerController::EquipWeapon(const struct FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ATFPlayerCharacter* TFCharacter = Cast<ATFPlayerCharacter>(ControlledPawn);
		if (TFCharacter)
		{
			TFCharacter->EquipButtonPressed();
		}
	}
}

void ATFPlayerController::WeaponAttackStarted(const struct FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ATFPlayerCharacter* TFCharacter = Cast<ATFPlayerCharacter>(ControlledPawn);
		if (TFCharacter)
		{
			TFCharacter->AttackButtonPressed();
		}
	}
}

void ATFPlayerController::WeaponAttackReleased(const struct FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ATFPlayerCharacter* TFCharacter = Cast<ATFPlayerCharacter>(ControlledPawn);
		if (TFCharacter)
		{
			TFCharacter->AttackButtonReleased();
		}
	}
}

void ATFPlayerController::SetHUDAmmo(int32 Ammo)
{
	TFHUD = TFHUD == nullptr ? Cast<ATFHUD>(GetHUD()) : TFHUD;
	
	bool bTFHUDValid = TFHUD && TFHUD->CharacterOverlay && TFHUD->CharacterOverlay->AmmoAmount;

	if (bTFHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		TFHUD->CharacterOverlay->AmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

// Ammo 0으로 초기화
void ATFPlayerController::ClientResetAmmo_Implementation()
{
	TFHUD = TFHUD == nullptr ? Cast<ATFHUD>(GetHUD()) : TFHUD;
	
	bool bTFHUDValid = TFHUD && TFHUD->CharacterOverlay && TFHUD->CharacterOverlay->AmmoAmount;

	if (bTFHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), 0);
		TFHUD->CharacterOverlay->AmmoAmount->SetText(FText::FromString(AmmoText));
	}
}


// Map 이동해도 유지할 Actor 넣는 함수
void ATFPlayerController::GetSeamlessTravelActorList(bool bToEntry, TArray<AActor*>& ActorList)
{
	Super::GetSeamlessTravelActorList(bToEntry, ActorList);

	// 이 컨트롤러 자체를 유지
	ActorList.Add(this);

	// PlayerState가 유효하면 유지 목록에 추가
	if (ATFMatchPlayerState* PS = GetPlayerState<ATFMatchPlayerState>())
	{
		ActorList.Add(PS);
	}
}


void ATFPlayerController::HandlePawnChanged(APawn* Old_Pawn, APawn* New_Pawn)
{
	if (!IsValid(HealthBar)) return;

	UTFPlayerHealthComponent* OldComp = nullptr;
	UTFPlayerHealthComponent* NewComp = nullptr;

	if (IsValid(Old_Pawn))
	{
		OldComp = Old_Pawn->FindComponentByClass<UTFPlayerHealthComponent>();
	}
	if (IsValid(New_Pawn))
	{
		NewComp = New_Pawn->FindComponentByClass<UTFPlayerHealthComponent>();
	}

	HealthBar->BindToHealthComponent(OldComp, NewComp);
}