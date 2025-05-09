// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameLiftServerSDK.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DSGameInstanceSubsystem.generated.h"

// GameInstanceSubsystem은 게임 전체에서 단 한 번만 생성되고 계속 유지
// GameMode는 레벨이 로드될 때마다 새로 생성
// 즉, 서버의 프로세스 수명 동안 지속되며, GameLift 초기화나 서버 상태 같은 전역 시스템 관리에 적합
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UDSGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UDSGameInstanceSubsystem();
	void InitGameLift(const FServerParameters& ServerParams);
 
	UPROPERTY(BlueprintReadOnly)
	bool bGameLiftInitialized;
 
private:
	void ParseCommandLinePort(int32& OutPort);
 
	FProcessParameters ProcessParameters;
};
