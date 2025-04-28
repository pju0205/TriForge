// Fill out your copyright notice in the Description page of Project Settings.


#include "DedicatedServers/Public/Game/DSGameMode.h"

DEFINE_LOG_CATEGORY(LogDS_GameMode);

void ADSGameMode::BeginPlay()
{
    Super::BeginPlay();

    
    // GameLift가 포함된 빌드에서만 InitGameLift 실행
#if WITH_GAMELIFT
    InitGameLift();
#endif
}

void ADSGameMode::InitGameLift()
{
    UE_LOG(LogDS_GameMode, Log, TEXT("Initializing the GameLift Server"));

    // GameLift 서버 SDK 모듈 로드
    FGameLiftServerSDKModule* GameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));

    // GameLift Anywhere 환경에서 사용할 서버 설정 구조체 생성
    // Anywhere, EC2 등등
    // EC2 : Elastic Cloud Compute
    FServerParameters ServerParameters;

    // 커맨드라인에서 서버 설정 읽어오기
    SetServerParameters(ServerParameters);

    // GameLift SDK 초기화
    GameLiftSdkModule->InitSDK(ServerParameters);


    // ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
    // 게임 세션 시작 이벤트 바인딩
    // ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
    auto OnGameSession = [=](Aws::GameLift::Server::Model::GameSession gameSession)
        {
            FString GameSessionId = FString(gameSession.GetGameSessionId());
            UE_LOG(LogDS_GameMode, Log, TEXT("GameSession Initializing: %s"), *GameSessionId);
        
            // 세션 활성화
            GameLiftSdkModule->ActivateGameSession();
        };
    ProcessParameters.OnStartGameSession.BindLambda(OnGameSession);

    // ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
    // 서버 종료 이벤트 바인딩
    // ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
    auto OnProcessTerminate = [=]()
        {
            UE_LOG(LogDS_GameMode, Log, TEXT("Game Server process is terminating."));
        
            GameLiftSdkModule->ProcessEnding();
        };
    ProcessParameters.OnTerminate.BindLambda(OnProcessTerminate);


    // ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
    // 헬스 체크 (서버 살아있는지 확인용)
    // ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
    auto OnHealthCheck = []()
        {
            UE_LOG(LogDS_GameMode, Log, TEXT("Performing Health Check"));
            return true;
        };
    ProcessParameters.OnHealthCheck.BindLambda(OnHealthCheck);


    // 기본 포트 설정 (커맨드라인에서 바꿀 수도 있음)
    int32 Port = FURL::UrlConfig.DefaultPort;
    ParseCommandLinePort(Port);
    ProcessParameters.port = Port;


    // 서버 로그 파일 경로 추가
    TArray<FString> LogFiles;
    LogFiles.Add(TEXT("FPSTemplate/Saved/Logs/FPSTemplate.log"));
    ProcessParameters.logParameters = LogFiles;

    // ProcessReady 호출해서 GameLift에 서버 준비 완료 알림
    UE_LOG(LogDS_GameMode, Log, TEXT("Calling Process Ready."));
    GameLiftSdkModule->ProcessReady(ProcessParameters);
}

void ADSGameMode::SetServerParameters(FServerParameters& OutServerParameters)
{
    // 커맨드라인에서 인증 토큰 읽어오기
    if (FParse::Value(FCommandLine::Get(), TEXT("-authtoken="), OutServerParameters.m_authToken))
    {
        UE_LOG(LogDS_GameMode, Log, TEXT("AUTH_TOKEN: %s"), *OutServerParameters.m_authToken)
    }

    // 커맨드라인에서 호스트 ID 읽어오기
    if (FParse::Value(FCommandLine::Get(), TEXT("-hostid="), OutServerParameters.m_hostId))
    {
        UE_LOG(LogDS_GameMode, Log, TEXT("HOST_ID: %s"), *OutServerParameters.m_hostId)
    }

    // 커맨드라인에서 플릿 ID 읽어오기
    if (FParse::Value(FCommandLine::Get(), TEXT("-fleetid="), OutServerParameters.m_fleetId))
    {
        UE_LOG(LogDS_GameMode, Log, TEXT("FLEET_ID: %s"), *OutServerParameters.m_fleetId)
    }

    // 커맨드라인에서 WebSocket URL 읽어오기
    if (FParse::Value(FCommandLine::Get(), TEXT("-websocketurl="), OutServerParameters.m_webSocketUrl))
    {
        UE_LOG(LogDS_GameMode, Log, TEXT("WEBSOCKET_URL: %s"), *OutServerParameters.m_webSocketUrl)
    }

    // 현재 프로세스 ID 저장
    OutServerParameters.m_processId = FString::Printf(TEXT("%d"), GetCurrentProcessId());
    UE_LOG(LogDS_GameMode, Log, TEXT("PID: %s"), *OutServerParameters.m_processId);
}

void ADSGameMode::ParseCommandLinePort(int32& OutPort)
{
    TArray<FString> CommandLineTokens;
    TArray<FString> CommandLineSwitches;

    // 커맨드라인 파싱 (예시: -port=7777)
    // ex) MyGame.exe -port=7777 -log -debug => ["MyGame.exe"], ["port=7777", "log", "debug"]
    FCommandLine::Parse(FCommandLine::Get(), CommandLineTokens, CommandLineSwitches);
    
    for (const FString& Switch : CommandLineSwitches)
    {
        FString Key;
        FString Value;

        // "key=value" 형식 분리
        if (Switch.Split("=", &Key, &Value))
        {
            if (Key.Equals(TEXT("port"), ESearchCase::IgnoreCase))
            {
                // 포트 번호 설정
                OutPort = FCString::Atoi(*Value);
                return;
            }
        }
    }
}
