// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/TFPlayerHealthComponent.h"

#include "Character/TFPlayerController.h"
#include "Net/UnrealNetwork.h"

UTFPlayerHealthComponent::UTFPlayerHealthComponent()
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	CurrentHealth = 100.f;
	MaxHealth = 100.f;
	DeathState = EDeathState::NotDead;
}

void UTFPlayerHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// Health 초기화 등 필요한 초기 작업
	CurrentHealth = MaxHealth;

	// Pawn 및 Controller 가져오기
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn)
	{
		APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
		if (PC)
		{
			ATFPlayerController* TFPC = Cast<ATFPlayerController>(PC);
			if (TFPC)
			{
				TFPC->SetHUDHealth(CurrentHealth, MaxHealth);
				OnHealthChanged.AddDynamic(TFPC, &ATFPlayerController::SetHUDHealth);
			}
		}
	}
}

void UTFPlayerHealthComponent::CalcDamage(float Amount, AActor* Instigator)
{
	float OldHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(CurrentHealth - Amount, 0.0f, MaxHealth);

	UE_LOG(LogTemp, Warning, TEXT("CalcDamage called: OldHealth=%f, NewHealth=%f"), OldHealth, CurrentHealth);

	
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	
	if (CurrentHealth <= 0.f)
	{
		StartDeath(Instigator);
	}
}

void UTFPlayerHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTFPlayerHealthComponent, DeathState);
	DOREPLIFETIME(UTFPlayerHealthComponent, CurrentHealth);
	DOREPLIFETIME(UTFPlayerHealthComponent, MaxHealth);
}

void UTFPlayerHealthComponent::StartDeath(AActor* Instigator)
{
	if (DeathState != EDeathState::NotDead)
	{
		return;
	}

	DeathState = EDeathState::DeathStarted;

	AActor* Owner = GetOwner();
	check(Owner);

	// TFPlayerChacter 에서 바인딩 되어 있는 함수 실행
	OnDeathStarted.Broadcast(Owner, Instigator);

	Owner->ForceNetUpdate();
}

void UTFPlayerHealthComponent::FinishDeath(AActor* Instigator)
{
	if (DeathState != EDeathState::DeathStarted)
	{
		return;
	}

	DeathState = EDeathState::DeathFinished;

	AActor* Owner = GetOwner();
	check(Owner);

	OnDeathFinished.Broadcast(Owner, Instigator);

	Owner->ForceNetUpdate();
}

void UTFPlayerHealthComponent::OnRep_Health()
{
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void UTFPlayerHealthComponent::OnRep_MaxHealth()
{
	OnMaxHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

// 서버-클라이언트 간 DeathState 상태를 정확히 맞추기 위한 검증 + 보정 함수
void UTFPlayerHealthComponent::OnRep_DeathState(EDeathState OldDeathState)
{
	const EDeathState NewDeathState = DeathState;		// DeathState는 새로 동기화된 서버의 값. NewDeathState로 백업해둠.
	DeathState = OldDeathState;							// 지금은 StartDeath와 FinishDeath에 의존하여 사망 상태를 변경

	if (OldDeathState > NewDeathState)
	{
		// 서버가 상태를 되돌리려고 할 경우 (클라가 더 앞서 있음) 되돌리지 않음
		return;
	}

	if (OldDeathState == EDeathState::NotDead)
	{
		if (NewDeathState == EDeathState::DeathStarted)
		{
			StartDeath(nullptr);
		}
		else if (NewDeathState == EDeathState::DeathFinished)
		{
			StartDeath(nullptr);
			FinishDeath(nullptr);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("HealthComponent: Invalid death transition [%d] -> [%d] for owner [%s]."),
				(uint8)OldDeathState, (uint8)NewDeathState, *GetNameSafe(GetOwner()));
		}
	}
	else if (OldDeathState == EDeathState::DeathStarted)
	{
		if (NewDeathState == EDeathState::DeathFinished)
		{
			FinishDeath(nullptr);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("HealthComponent: Invalid death transition [%d] -> [%d] for owner [%s]."),
				(uint8)OldDeathState, (uint8)NewDeathState, *GetNameSafe(GetOwner()));
		}
	}
}