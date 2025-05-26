// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameLiftServerSDK.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DSGameInstanceSubsystem.generated.h"

class UGameSessionsManager;
struct FDSGameSession;
// GameInstanceSubsystem은 게임 전체에서 단 한 번만 생성되고 계속 유지
// GameMode는 레벨이 로드될 때마다 새로 생성
// 즉, 서버의 프로세스 수명 동안 지속되며, GameLift 초기화나 서버 상태 같은 전역 시스템 관리에 적합
/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameSessionsUpdated);

UCLASS()
class DEDICATEDSERVERS_API UDSGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// 기존에 있던 코드들
	UDSGameInstanceSubsystem();
	void InitGameLift(const FServerParameters& ServerParams);
 
	UPROPERTY(BlueprintReadOnly)
	bool bGameLiftInitialized;
	
	/*// ✨ Lazy Init 방식으로 접근하는 함수 추가
	UFUNCTION(BlueprintCallable)
	UGameSessionsManager* GetGameSessionsManager();

	UPROPERTY()
	TObjectPtr<UGameSessionsManager> GameSessionsManager;

	// 세션 캐시
	TMap<FString, FDSGameSession> CachedGameSessions;

	UFUNCTION()
	void HandleGameSessionCreated(const FDSGameSession& Session);
	
	// UI 업데이트용 델리게이트
	UPROPERTY(BlueprintAssignable)
	FOnGameSessionsUpdated OnGameSessionsUpdated;

	// 서버에서 세션 목록 갱신
	void UpdateSessionsFromServer();

	// 캐시에 추가/삭제/초기화하는 헬퍼들
	void AddOrUpdateSession(const FDSGameSession& Session);
	void RemoveSessionById(const FString& SessionId);
	void ClearSessions();*/
 
private:
	void ParseCommandLinePort(int32& OutPort);
 
	FProcessParameters ProcessParameters;


	/*void InitGameSessionsManagerIfNeeded(); // 내부 함수*/
};
