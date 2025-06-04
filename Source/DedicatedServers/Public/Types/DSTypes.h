#pragma once
 
#include "DSTypes.generated.h"


//
// 카운트다운 타이머 상태 열거형
//
UENUM(BlueprintType)
enum class ECountdownTimerState : uint8
{
	NotStarted UMETA(DisplayName = "Not Started"),
	Started UMETA(DisplayName = "Started"),
	Paused UMETA(DisplayName = "Paused"),
	Stopped UMETA(DisplayName = "Stopped")
};


//
// 타이머의 종류를 정의 열거형 (어디서 쓰이는 타이머인지 명시)
//
UENUM(BlueprintType)
enum class ECountdownTimerType : uint8
{
	LobbyCountdown UMETA(DisplayName = "Lobby Countdown"),
	PreRound UMETA(DisplayName = "Pre Round"),
	Round UMETA(DisplayName = "Round"),
	PostRound UMETA(DisplayName = "Post Round"),
	PostMatch UMETA(DisplayName = "Post Match"),
	None UMETA(DisplayName = "None"),

	ForceMatchEnded UMETA(DisplayName = "ForceMatchEnded")
};

//
// 게임 매치 상태 열거형
//
UENUM()
enum class EMatchStatus : uint8
{
	WaitingForPlayers,
	PreRound,
	Round,
	PostRound,
	PostMatch,
	GameOver,
	SeamlessTravelling
};

//
// 게임 로비 상태 열거형
//
UENUM()
enum class ELobbyStatus : uint8
{
	WaitingForPlayers,
	CountdownToSeamlessTravel,
	SeamlessTravelling
};


//
// 타이머에 대한 전체 정보 및 컨트롤 핸들을 묶어놓은 구조체
//
USTRUCT(BlueprintType)
struct FCountdownTimerHandle
{
	GENERATED_BODY()
 
	UPROPERTY(BlueprintReadWrite)
	ECountdownTimerState State = ECountdownTimerState::NotStarted;
 
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	ECountdownTimerType Type = ECountdownTimerType::None;
 
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float CountdownTime = 0.f;
 
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float CountdownUpdateInterval = 1.f;
 
	FTimerHandle TimerFinishedHandle{};		// 종료 핸들
	FTimerHandle TimerUpdateHandle{};		// 업데이트용 핸들
	FTimerDelegate TimerFinishedDelegate{};	// 타이머 완료 시 바인딩 델리게이트
	FTimerDelegate TimerUpdateDelegate{};	// 업데이트용 델리게이트
};


//
// == 연산자 오버로딩
// 두 FCountdownTimerHandle이 같은 종류의 타이머(Type)인지 비교
//
inline bool operator==(const FCountdownTimerHandle& lhs, const FCountdownTimerHandle& rhs)
{
	return lhs.Type == rhs.Type;
}