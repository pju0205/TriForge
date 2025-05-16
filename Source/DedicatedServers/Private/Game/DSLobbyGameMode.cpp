// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/DSLobbyGameMode.h"
#include "Game/DSGameInstanceSubsystem.h"
#include "DedicatedServers/DedicatedServers.h"
#include "Game/DSGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Lobby/LobbyPlayerInfo.h"
#include "Lobby/LobbyState.h"
#include "Player/DSPlayerController.h"


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

    InitGameLift();     // Lobby 상태에서 GameLift PlayerSessions 초기화 시키기
}

// 플레이어가 로그인
void ADSLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
    
    CheckAndStartLobbyCountdown();
}

// SeamlessTravel로 이동 중일 때 플레이어 컨트롤러 초기화 시 호출
void ADSLobbyGameMode::InitSeamlessTravelPlayer(AController* NewController)
{
    Super::InitSeamlessTravelPlayer(NewController);
 
    CheckAndStartLobbyCountdown();

    // 로비 상태가 맵 변경중이 아니라면 실행
    if (LobbyStatus != ELobbyStatus::SeamlessTravelling)
    {
        AddPlayerInfoToLobbyState(NewController);
    }
}

// 플레이어가 로그아웃
void ADSLobbyGameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);
 
    CheckAndStopLobbyCountdown();
    RemovePlayerSession(Exiting);

    // 로비 상태가 맵 변경중이 아니라면 실행
    if (LobbyStatus != ELobbyStatus::SeamlessTravelling)
    {
        RemovePlayerInfoFromLobbyState(Exiting);
    }
}

// GameMode에서 게임에 참여하려고 시도할 때 가장 먼저 호출 됨
void ADSLobbyGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
    Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

    /*
    void ADSLobbyGameMode::PreLogin(
    const FString& Options,             // 접속할 때 넘겨진 옵션 문자열 (?key=value&key2=value2 형식)
    const FString& Address,             // 클라이언트 IP 주소
    const FUniqueNetIdRepl& UniqueId,   // 네트워크 ID (로그인 시스템에서 쓰이는 유저 ID)
    FString& ErrorMessage               // 에러 메시지를 설정하면 접속 거부됨 (비워두면 통과)
    )
    */

    // 정보 저장
    const FString PlayerSessionId = UGameplayStatics::ParseOption(Options, TEXT("PlayerSessionId"));
    const FString Username = UGameplayStatics::ParseOption(Options, TEXT("Username"));

    // 정보 가지고 해당 함수 실행
    TryAcceptPlayerSession(PlayerSessionId, Username, ErrorMessage);
}

// Lobby로 들어온 player 정보 초기화시키는 함수
FString ADSLobbyGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
    FString InitializedString = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

    const FString PlayerSessionId = UGameplayStatics::ParseOption(Options, TEXT("PlayerSessionId"));
    const FString Username = UGameplayStatics::ParseOption(Options, TEXT("Username"));

    if (ADSPlayerController* DSPlayerController = Cast<ADSPlayerController>(NewPlayerController); IsValid(DSPlayerController))
    {
        DSPlayerController->PlayerSessionId = PlayerSessionId;
        DSPlayerController->Username = Username;
    }
    
    if (LobbyStatus != ELobbyStatus::SeamlessTravelling)
    {
        AddPlayerInfoToLobbyState(NewPlayerController);         // playerSessionID, Username을 저장한 값으로 LobbyState 생성
    }

    return InitializedString;
}

// playerInfo 상태 추가 함수
void ADSLobbyGameMode::AddPlayerInfoToLobbyState(AController* Player) const
{
    ADSPlayerController* DSPlayerController = Cast<ADSPlayerController>(Player);
    ADSGameState* DSGameState = GetGameState<ADSGameState>();
    if (IsValid(DSGameState) && IsValid(DSGameState->LobbyState) && IsValid(DSPlayerController))
    {
        FLobbyPlayerInfo PlayerInfo(DSPlayerController->Username);
        DSGameState->LobbyState->AddPlayerInfo(PlayerInfo);
    }
}

// playerInfo 상태 삭제 함수
void ADSLobbyGameMode::RemovePlayerInfoFromLobbyState(AController* Player) const
{
    ADSPlayerController* DSPlayerController = Cast<ADSPlayerController>(Player);
    ADSGameState* DSGameState = GetGameState<ADSGameState>();
    if (IsValid(DSGameState) && IsValid(DSGameState->LobbyState) && IsValid(DSPlayerController))
    {
        DSGameState->LobbyState->RemovePlayerInfo(DSPlayerController->Username);
    }
}


// playerSession 접근
void ADSLobbyGameMode::TryAcceptPlayerSession(const FString& PlayerSessionId, const FString& Username, FString& OutErrorMessage)
{
    // nullptr 확인
    if (PlayerSessionId.IsEmpty() || Username.IsEmpty())
    {
        OutErrorMessage = TEXT("PlayerSessionId and/or Username invalid.");
        return;
    }


#if WITH_GAMELIFT
    // // PlayerSessionId 기반으로 GameLift에 세션 정보 요청
    Aws::GameLift::Server::Model::DescribePlayerSessionsRequest DescribePlayerSessionsRequest;
    DescribePlayerSessionsRequest.SetPlayerSessionId(TCHAR_TO_ANSI(*PlayerSessionId));

    // GameLift로부터 세션 정보 요청 결과 받아옴
    const auto& DescribePlayerSessionsOutcome = Aws::GameLift::Server::DescribePlayerSessions(DescribePlayerSessionsRequest);
    if (!DescribePlayerSessionsOutcome.IsSuccess())
    {
        OutErrorMessage = TEXT("DescribePlayerSessions failed.");
        return;
    }

    // 결과에서 PlayerSession 배열 꺼냄
    const auto& DescribePlayerSessionsResult = DescribePlayerSessionsOutcome.GetResult();
    int32 Count = 0;
    const Aws::GameLift::Server::Model::PlayerSession* PlayerSessions = DescribePlayerSessionsResult.GetPlayerSessions(Count);
    if (PlayerSessions == nullptr || Count == 0)
    {
        OutErrorMessage = TEXT("GetPlayerSessions failed.");
        return;
    }

    // 여러 세션 중 일치하는 플레이어 ID(=Username)를 찾기
    for (int32 i = 0; i < Count; i++)
    {
        const Aws::GameLift::Server::Model::PlayerSession& PlayerSession = PlayerSessions[i];

        // 유저명이 다르면 스킵
        if (!Username.Equals(PlayerSession.GetPlayerId())) continue;

        // 세션 상태가 RESERVED가 아니면 거절 (접속 허용 전 상태만 받아들임)
        if (PlayerSession.GetStatus() != Aws::GameLift::Server::Model::PlayerSessionStatus::RESERVED)
        {
            OutErrorMessage = FString::Printf(TEXT("Session for %s not RESERVED; Fail PreLogin."), *Username);
            return;
        }
        
        // 세션 수락 시도
        const auto& AcceptPlayerSessionOutcome = Aws::GameLift::Server::AcceptPlayerSession(TCHAR_TO_ANSI(*PlayerSessionId));

        // 성공 여부에 따라 에러 메시지 설정
        OutErrorMessage = AcceptPlayerSessionOutcome.IsSuccess() ? "" : FString::Printf(TEXT("Failed to accept player session for %s"), *Username);
    }
#endif
}

void ADSLobbyGameMode::CheckAndStartLobbyCountdown()
{
    if (ADSGameState* GS = GetGameState<ADSGameState>())
    {
        if (ALobbyState* LobbyState = GS->LobbyState)
        {
            // 최소 인원 확인
            if (GetNumPlayers() < MinPlayers || LobbyStatus != ELobbyStatus::WaitingForPlayers)
            {
                CheckAndStopLobbyCountdown();
                return;
            }

            // 전부 준비됐는지 확인
            if (!LobbyState->AreAllPlayersReady())
            {
                CheckAndStopLobbyCountdown();
                return;
            }

            // 모두 준비 완료!
            LobbyStatus = ELobbyStatus::CountdownToSeamlessTravel;
            StartCountdownTimer(LobbyCountdownTimer);
        }
    }
}

// 플레이어가 빠져서 인원이 부족한 경우, 카운트다운 취소
void ADSLobbyGameMode::CheckAndStopLobbyCountdown()
{
    // 현재 인원 -1이 최소 인원보다 적고, 카운트다운 중일 경우
    if (GetNumPlayers() - 1 < MinPlayers && LobbyStatus == ELobbyStatus::CountdownToSeamlessTravel)
    {
        LobbyStatus = ELobbyStatus::WaitingForPlayers;      // 상태 복구
        StopCountdownTimer(LobbyCountdownTimer);         // 타이머 멈춤
    }
}

// 카운트다운 종료 시 호출되는 함수 (부모 함수도 호출)
void ADSLobbyGameMode::OnCountdownTimerFinished(ECountdownTimerType Type)
{
    Super::OnCountdownTimerFinished(Type);

    // 로비 카운트다운이 끝나면
    if (Type == ECountdownTimerType::LobbyCountdown)
    {
        StopCountdownTimer(LobbyCountdownTimer);
        LobbyStatus = ELobbyStatus::SeamlessTravelling;     // 상태 변경: Seamless Travel 중
        TrySeamlessTravel(MapToTravelTo);
    }
}


void ADSLobbyGameMode::InitGameLift()
{
    if (UGameInstance* GameInstance = GetGameInstance(); IsValid(GameInstance))
    {
        if (DSGameInstanceSubsystem = GameInstance->GetSubsystem<UDSGameInstanceSubsystem>(); IsValid(DSGameInstanceSubsystem))
        {
            FServerParameters ServerParameters;
            SetServerParameters(ServerParameters);

            // GameInstanceSubsystem 에서 서버 실행
            DSGameInstanceSubsystem->InitGameLift(ServerParameters);
        }
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
