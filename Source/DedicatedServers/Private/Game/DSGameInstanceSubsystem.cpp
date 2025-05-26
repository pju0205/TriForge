// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/DSGameInstanceSubsystem.h"
#include "DedicatedServers/DedicatedServers.h"		// DedicatedServers.h 에 정의한 Log 쓰려고 가져옴
#include "UI/HTTP/HTTPRequestTypes.h"
#include "UI/GameSessions/GameSessionsManager.h"

UDSGameInstanceSubsystem::UDSGameInstanceSubsystem()
{
 	bGameLiftInitialized = false;
}

void UDSGameInstanceSubsystem::InitGameLift(const FServerParameters& ServerParams)
{
	if (bGameLiftInitialized) return;

	// WITH_GAMELIFT는 Build.cs에서 정의된 매크로
	// 이 매크로가 true일 때만, GameLift 관련 코드를 포함
#if WITH_GAMELIFT
	UE_LOG(LogDedicatedServers, Log, TEXT("Initializing the GameLift Server"));

	// GameLift 서버 SDK 모듈 로드
    FGameLiftServerSDKModule* GameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));
	GameLiftSdkModule->InitSDK(ServerParams);		// GameLift SDK 초기화

	// ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
	// 게임 세션 시작 이벤트 바인딩
	// ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
	auto OnGameSession = [=](Aws::GameLift::Server::Model::GameSession gameSession)
	{
		FString GameSessionId = FString(gameSession.GetGameSessionId());
		UE_LOG(LogDedicatedServers, Log, TEXT("GameSession Initializing: %s"), *GameSessionId);
		GameLiftSdkModule->ActivateGameSession();		// 세션 활성화
	};

	ProcessParameters.OnStartGameSession.BindLambda(OnGameSession);
	
	// ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
	// 서버 종료 이벤트 바인딩
	// ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
	auto OnProcessTerminate = [=]()
	{
		UE_LOG(LogDedicatedServers, Log, TEXT("Game Server process is terminating."));
		GameLiftSdkModule->ProcessEnding();
	};

	ProcessParameters.OnTerminate.BindLambda(OnProcessTerminate);

	// ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
	// 헬스 체크 (서버 살아있는지 확인용)
	// ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
	auto OnHealthCheck = []() 
	{
		UE_LOG(LogDedicatedServers, Log, TEXT("Performing Health Check"));
		return true;
	};

	ProcessParameters.OnHealthCheck.BindLambda(OnHealthCheck);

	// 기본 포트 설정 (커맨드라인에서 바꿀 수도 있음)
	int32 Port = FURL::UrlConfig.DefaultPort;
	ParseCommandLinePort(Port);	// 밑 함수 실행

	ProcessParameters.port = Port;

	// 서버 로그 파일 경로 추가
	TArray<FString> LogFiles;
	LogFiles.Add(TEXT("FPSTemplate/Saved/Logs/FPSTemplate.log"));
	ProcessParameters.logParameters = LogFiles;

	// ProcessReady 호출해서 GameLift에 서버 준비 완료 알림
	UE_LOG(LogDedicatedServers, Log, TEXT("Calling Process Ready."));
	GameLiftSdkModule->ProcessReady(ProcessParameters);
	
#endif
	bGameLiftInitialized = true;
}


void UDSGameInstanceSubsystem::ParseCommandLinePort(int32& OutPort)
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
			if (Key.Equals(TEXT("port"), ESearchCase::IgnoreCase) && Value.IsNumeric())
			{
				// 포트 번호 설정
				LexFromString(OutPort, *Value);
				return;
			}
		}
	}
}

/*void UDSGameInstanceSubsystem::InitGameSessionsManagerIfNeeded()
{
	if (!GameSessionsManager)
	{
		GameSessionsManager = NewObject<UGameSessionsManager>(this, UGameSessionsManager::StaticClass());
		GameSessionsManager->AddToRoot();
		GameSessionsManager->OnGameSessionCreated.AddDynamic(this, &UDSGameInstanceSubsystem::HandleGameSessionCreated);
		UE_LOG(LogTemp, Log, TEXT("GameSessionsManager Created and Initialized"));
	}
}

UGameSessionsManager* UDSGameInstanceSubsystem::GetGameSessionsManager()
{
	InitGameSessionsManagerIfNeeded();
	return GameSessionsManager;
}


void UDSGameInstanceSubsystem::HandleGameSessionCreated(const FDSGameSession& Session)
{
	AddOrUpdateSession(Session);	// 내부적으로 UI 업데이트도 트리거됨
}

void UDSGameInstanceSubsystem::UpdateSessionsFromServer()
{
	// 서버 API 호출을 통해 GameSessionList를 받아옴 (예: HTTP or WebSocket)
    
	// 예시: HTTP로 세션 리스트 받아오기
	// 요청 응답 후 아래처럼 업데이트한다고 가정

	TArray<FDSGameSession> ServerSessions; // <- 서버 응답

	// 캐시 갱신
	CachedGameSessions.Empty();

	for (const FDSGameSession& Session : ServerSessions)
	{
		CachedGameSessions.Add(Session.GameSessionId, Session);
	}

	// UI에 알림
	OnGameSessionsUpdated.Broadcast();
}

void UDSGameInstanceSubsystem::AddOrUpdateSession(const FDSGameSession& Session)
{
	CachedGameSessions.Add(Session.GameSessionId, Session);
	OnGameSessionsUpdated.Broadcast();
}

void UDSGameInstanceSubsystem::RemoveSessionById(const FString& SessionId)
{
	if (CachedGameSessions.Remove(SessionId) > 0)
	{
		OnGameSessionsUpdated.Broadcast();
	}
}

void UDSGameInstanceSubsystem::ClearSessions()
{
	CachedGameSessions.Empty();
	OnGameSessionsUpdated.Broadcast();
}*/
