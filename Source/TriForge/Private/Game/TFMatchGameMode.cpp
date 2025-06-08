// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TFMatchGameMode.h"

#include "Character/TFPlayerController.h"
#include "Character/Component/TFPlayerHealthComponent.h"
#include "Game/TFMatchGameState.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/DSPlayerController.h"
#include "PlayerState/TFMatchPlayerState.h"
#include "Types/TFTypes.h"
#include "Weapon/TFWeapon.h"

ATFMatchGameMode::ATFMatchGameMode()
{
	bUseSeamlessTravel = true;	// GameMode, GameState, PlayerController, PlayerState는 그대로 유지

	bIsEndedMatch = false;
}

void ATFMatchGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATFMatchGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void ATFMatchGameMode::HandleRoundEnd(APlayerController* Loser, APlayerController* Winner)
{
	if (MatchStatus != EMatchStatus::Round) return;
	
	// Winner가 null이면 생존자 찾아서 할당 (낙사와 같은 경우)
	if (!Winner)
	{
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			APlayerController* PC = It->Get();
			if (!IsValid(PC) || PC == Loser) continue;

			ATFPlayerController* TFPC = Cast<ATFPlayerController>(PC);
			if (IsValid(TFPC) && TFPC->bPawnAlive)
			{
				Winner = TFPC;
				break;
			}
		}
	}

	if (Winner) HandleRoundWinner(Winner);
	if (Loser) HandleRoundLoser(Loser);
	
	// 게임 승리 조건 확인
	ATFMatchPlayerState* WinnerPS = Cast<ATFMatchPlayerState>(Winner ? Winner->PlayerState : nullptr);
	if (WinnerPS && WinnerPS->RoundWins >= MaxRound)	// 2번 이상 라운드 승리시
	{
		HandleMatchWin(Loser, Winner);
		bIsEndedRound = true;
	}
	else
	{
		ATFPlayerController* TFPCW = Cast<ATFPlayerController>(Winner);
		ATFPlayerController* TFPCL = Cast<ATFPlayerController>(Loser);

		if (IsValid(TFPCW) && IsValid(TFPCL))
		{
			TFPCW->ClientShowDrawWidget(ERoundResult::Win);
			TFPCL->ClientShowDrawWidget(ERoundResult::Loss);
			
			StopCountdownTimer(RoundTimer);
			MatchStatus = EMatchStatus::PostRound;
			StartCountdownTimer(PostRoundTimer);
		}
	}
}

void ATFMatchGameMode::HandleMatchWin(APlayerController* Loser, APlayerController* Winner)
{
	ATFMatchPlayerState* WinnerPS = Winner ? Cast<ATFMatchPlayerState>(Winner->PlayerState) : nullptr;
	ATFMatchPlayerState* LoserPS = Loser ? Cast<ATFMatchPlayerState>(Loser->PlayerState) : nullptr;
	
	if (WinnerPS)
	{
		WinnerPS->AddMatchScore();	// PlayerState 값 증가 (기록용)
		WinnerPS->AddMatchResult(true);
	}

	if (LoserPS)
	{
		LoserPS->AddMatchResult(false);
	}

	// MatchWin가 2회 이상이면
	if (WinnerPS && WinnerPS->MyMatchWins >= MaxMatch)
	{
		bIsEndedMatch = true;
	}
	
	// MaxMatch 횟수 다 채웠으면 Winner 승리로 간주
	if (bIsEndedMatch)
	{
		ATFMatchGameState* GS = GetGameState<ATFMatchGameState>();
		if (IsValid(GS))
		{
			GS->UpdateLeader(); // 최종 승자 업데이트
		}
		
		// 완전 종료 게임 끝
		WinnerPS->IsTheWinner();			// 승리자로 기록
		OnMatchEnded();						// 이게 모두한테 실행이 되나? 일단 보류
		
		StopCountdownTimer(RoundTimer);
		MatchStatus = EMatchStatus::GameOver;
		StartCountdownTimer(ForceMatchEndedTimer);
	}
	else
	{
		bIsEndedRound = false;
		StopCountdownTimer(RoundTimer);
		MatchStatus = EMatchStatus::PostMatch;
		StartCountdownTimer(PostMatchTimer);
		
	}
}

void ATFMatchGameMode::OnCountdownTimerFinished(ECountdownTimerType Type)
{
	Super::OnCountdownTimerFinished(Type);

	// Round 관련
	if (Type == ECountdownTimerType::PreRound)
	{
		StopCountdownTimer(PreRoundTimer);
		MatchStatus = EMatchStatus::Round;
		StartCountdownTimer(RoundTimer);
		SetClientInputEnabled(true);
	}
	if (Type == ECountdownTimerType::Round)
	{
		StopCountdownTimer(RoundTimer);
		RoundTimeOutCheck();						// 시간 안에 결정 안나면 실행
	}
	if (Type == ECountdownTimerType::PostRound)
	{
		StopCountdownTimer(PostRoundTimer);
		MatchStatus = EMatchStatus::PreRound;
		SetClientInputEnabled(false);
		DestroyAllDroppedWeapons();
		PrepareNextRound();								// 다음 라운드 준비
	}
	
	// Match 관련
	if (Type == ECountdownTimerType::PostMatch)			// Round 2번 승리 시 실행 타이머
	{
		StopCountdownTimer(PostMatchTimer);
		MatchStatus = EMatchStatus::SeamlessTravelling;
		SetClientInputEnabled(false);
		PlayerEliminated();
		NextRandomTravelMap();							// 다음 전투 맵 이동
	}
}

void ATFMatchGameMode::OnMatchEnded()
{
	Super::OnMatchEnded();

	// 사용한 Map 비우기
	UsedCombatMapPaths.Empty();
}

// 다음 라운드 넘어 갈 때 (기존 맵 유지, Pawn만 새로 생성, Timer 새로 시작)
void ATFMatchGameMode::PrepareNextRound()
{
	// player 삭제해야 되는 데이터 삭제
	PlayerEliminated();

	// 라운드 타이머 시작
	StartCountdownTimer(PreRoundTimer);
}

// TFGameMode에서 실행
void ATFMatchGameMode::PlayerEliminated()
{
	
}

// Map상에 존재하는 Weapon 삭제하는 함수
void ATFMatchGameMode::DestroyAllDroppedWeapons()
{
	UWorld* World = GetWorld();
	if (!World) return;

	TArray<AActor*> FoundWeapons;
	UGameplayStatics::GetAllActorsOfClass(World, ATFWeapon::StaticClass(), FoundWeapons);

	for (AActor* Weapon : FoundWeapons)
	{
		if (Weapon)
		{
			Weapon->Destroy();
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Dropped Weapons Destroyed: %d"), FoundWeapons.Num());
}

// 승리자 스코어, 위젯 관리
void ATFMatchGameMode::HandleRoundWinner(APlayerController* Winner)
{
	if (!IsValid(Winner)) return;

	if (ATFMatchPlayerState* WinnerPS = Cast<ATFMatchPlayerState>(Winner->PlayerState))
	{
		WinnerPS->RoundWins++;
		WinnerPS->AddRoundScore();
		WinnerPS->AddKill();
		WinnerPS->AddRoundResult(true);
	}
}

// 패배자 스코어, 위젯 관리
void ATFMatchGameMode::HandleRoundLoser(APlayerController* Loser)
{
	if (!IsValid(Loser)) return;

	if (ATFMatchPlayerState* LoserPS = Cast<ATFMatchPlayerState>(Loser->PlayerState))
	{
		LoserPS->AddDeath();
		LoserPS->AddRoundResult(false);
	}
}

// 무승부 시 둘다 무승부 위젯 띄우기
void ATFMatchGameMode::HandleDrawRound()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ATFPlayerController* PC = Cast<ATFPlayerController>(It->Get());
		if (IsValid(PC))
		{
			PC->ClientShowDrawWidget(ERoundResult::Draw);
		}
	}
}

// 라운드 타이머 종료시 실행 (HP 검사 및 승패 결정)
void ATFMatchGameMode::RoundTimeOutCheck()
{
	if (!HasAuthority()) // 추가
	{
		UE_LOG(LogTemp, Error, TEXT("서버 아님. RoundTimeOutCheck()"));
		return;
	}
	
	struct FPlayerHealthInfo
	{
		ATFPlayerController* Controller = nullptr;
		float Health = 0.f;
	};

	TArray<FPlayerHealthInfo> AlivePlayers;

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ATFPlayerController* TFPC = Cast<ATFPlayerController>(It->Get());
		if (!IsValid(TFPC) || !TFPC->bPawnAlive) continue;

		APawn* Pawn = TFPC->GetPawn();
		if (!IsValid(Pawn)) continue;

		// HealthComponent 가져오기
		UTFPlayerHealthComponent* HealthComp = Pawn->FindComponentByClass<UTFPlayerHealthComponent>();
		if (!IsValid(HealthComp)) continue;

		FPlayerHealthInfo Info;
		Info.Controller = TFPC;
		Info.Health = HealthComp->GetCurrentHealth();  // 혹은 HealthComp->CurrentHealth
		AlivePlayers.Add(Info);
	}

	if (AlivePlayers.Num() < 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not enough alive players to compare."));
		return;
	}

	// 체력 비교
	FPlayerHealthInfo& P1 = AlivePlayers[0];
	FPlayerHealthInfo& P2 = AlivePlayers[1];

	// 체력이 같을 때 Draw => 라운드 한번 더
	if (P1.Health == P2.Health)
	{
		StopCountdownTimer(RoundTimer);
		MatchStatus = EMatchStatus::PostRound;
		StartCountdownTimer(PostRoundTimer);
		HandleDrawRound();
		return;
	}

	// 체력이 높은 쪽을 승리 처리
	ATFPlayerController* Winner = (P1.Health > P2.Health) ? P1.Controller : P2.Controller;
	ATFPlayerController* Loser  = (P1.Health > P2.Health) ? P2.Controller : P1.Controller;

	HandleRoundEnd(Loser, Winner);
}
	