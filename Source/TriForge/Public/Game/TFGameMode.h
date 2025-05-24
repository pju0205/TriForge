// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/TFMatchGameMode.h"
#include "TFGameMode.generated.h"

// 캐릭터 관련 관리 GameMode
/**
 * 
 */
UCLASS()
class TRIFORGE_API ATFGameMode : public ATFMatchGameMode
{
	GENERATED_BODY()
public:
	ATFGameMode();
	virtual void Tick(float DeltaTime) override;

	virtual void PlayerEliminated() override;

	// 스폰 중복 방지를 위한 PlayerStart 선택 함수
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	
	
	UPROPERTY()
	TMap<APlayerController*, FTimerHandle> Timers;
	
protected:
	virtual void OnCountdownTimerFinished(ECountdownTimerType Type) override;
		
	virtual void OnMatchEnded() override;	// Match 끝남을 알림

private:
	// 이미 사용한 PlayerStart 저장용
	UPROPERTY()
	TSet<AActor*> UsedPlayerStarts;
};
