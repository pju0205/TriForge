// Fill out your copyright notice in the Description page of Project Settings.


#include "DedicatedServers/Public/Game/DSLobbyGameMode.h"
#include "Game/DSGameInstanceSubsystem.h"
#include "DedicatedServers/DedicatedServers.h"

ADSLobbyGameMode::ADSLobbyGameMode()
{
    // SeamlessTravel을 사용하여 로딩 없이 레벨 전환
    bUseSeamlessTravel = true;
    LobbyStatus = ELobbyStatus::WaitingForPlayers;      // 초기 상태는 플레이어 대기
    MinPlayers = 1;                                     // 최소 시작 인원 설정
    LobbyCountdownTimer.Type = ECountdownTimerType::LobbyCountdown; // 타이머 타입 설정
}


void ADSLobbyGameMode::BeginPlay()
{
    Super::BeginPlay();

    InitGameLift();
}

void ADSLobbyGameMode::InitGameLift()
{
    if (UGameInstance* GameInstance = GetGameInstance(); IsValid(GameInstance))
    {
        if (DSGameInstanceSubsystem = GameInstance->GetSubsystem<UDSGameInstanceSubsystem>(); IsValid(DSGameInstanceSubsystem))
        {
            FServerParameters ServerParameters;
            SetServerParameters(ServerParameters);
            DSGameInstanceSubsystem->InitGameLift(ServerParameters);
        }
    }
}

// 카운트다운 종료 시 호출되는 함수 (부모 함수도 호출)
void ADSLobbyGameMode::OnCountdownTimerFinished(ECountdownTimerType Type)
{
    Super::OnCountdownTimerFinished(Type);

    // 로비 카운트다운이 끝나면
    if (Type == ECountdownTimerType::LobbyCountdown)
    {
        LobbyStatus = ELobbyStatus::SeamlessTravelling;                  // 상태 변경: Seamless Travel 중
        TrySeamlessTravel(MapToTravelTo);                             // 대상 맵으로 서버 이동
    }
}

// 플레이어가 로그인
void ADSLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    // 최소 인원 이상이고, 대기 상태인 경우 카운트다운 시작
    if (GetNumPlayers() >= MinPlayers && LobbyStatus == ELobbyStatus::WaitingForPlayers)
    {
        LobbyStatus = ELobbyStatus::CountdownToSeamlessTravel;  // 상태 변경: 카운트다운 중
        StartCountdownTimer(LobbyCountdownTimer);            // 카운트다운 타이머 시작
    }
}


// 플레이어가 로그아웃
void ADSLobbyGameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);
 
    CancelCountdown();
}


// SeamlessTravel로 이동 중일 때 플레이어 컨트롤러 초기화 시 호출
void ADSLobbyGameMode::InitSeamlessTravelPlayer(AController* NewController)
{
    Super::InitSeamlessTravelPlayer(NewController);
 
    CancelCountdown();  // 이동 중 카운트다운 취소할지 확인
}


// 플레이어가 빠져서 인원이 부족한 경우, 카운트다운 취소
void ADSLobbyGameMode::CancelCountdown()
{
    // 현재 인원 -1이 최소 인원보다 적고, 카운트다운 중일 경우
    if (GetNumPlayers() - 1 < MinPlayers && LobbyStatus == ELobbyStatus::CountdownToSeamlessTravel)
    {
        LobbyStatus = ELobbyStatus::WaitingForPlayers;      // 상태 복구
        StopCountdownTimer(LobbyCountdownTimer);         // 타이머 멈춤
    }
}
 

void ADSLobbyGameMode::SetServerParameters(FServerParameters& OutServerParameters)
{
    // 커맨드라인에서 인증 토큰 읽어오기
    if (FParse::Value(FCommandLine::Get(), TEXT("-authtoken="), OutServerParameters.m_authToken))
    {
        UE_LOG(LogDedicatedServers, Log, TEXT("AUTH_TOKEN: %s"), *OutServerParameters.m_authToken)
    }

    // 커맨드라인에서 호스트 ID 읽어오기
    if (FParse::Value(FCommandLine::Get(), TEXT("-hostid="), OutServerParameters.m_hostId))
    {
        UE_LOG(LogDedicatedServers, Log, TEXT("HOST_ID: %s"), *OutServerParameters.m_hostId)
    }

    // 커맨드라인에서 플릿 ID 읽어오기
    if (FParse::Value(FCommandLine::Get(), TEXT("-fleetid="), OutServerParameters.m_fleetId))
    {
        UE_LOG(LogDedicatedServers, Log, TEXT("FLEET_ID: %s"), *OutServerParameters.m_fleetId)
    }

    // 커맨드라인에서 WebSocket URL 읽어오기
    if (FParse::Value(FCommandLine::Get(), TEXT("-websocketurl="), OutServerParameters.m_webSocketUrl))
    {
        UE_LOG(LogDedicatedServers, Log, TEXT("WEBSOCKET_URL: %s"), *OutServerParameters.m_webSocketUrl)
    }

    // 현재 프로세스 ID 저장
    OutServerParameters.m_processId = FString::Printf(TEXT("%d"), GetCurrentProcessId());
    UE_LOG(LogDedicatedServers, Log, TEXT("PID: %s"), *OutServerParameters.m_processId);
}