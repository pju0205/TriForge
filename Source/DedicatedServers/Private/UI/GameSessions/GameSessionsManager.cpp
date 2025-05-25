// Fill out your copyright notice in the Description page of Project Settings.


#include "DedicatedServers/Public/UI/GameSessions/GameSessionsManager.h"

#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Data/API/APIData.h"
#include "GameFramework/PlayerState.h"
#include "GameplayTags/DedicatedServersTags.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Kismet/GameplayStatics.h"
#include "UI/HTTP/HTTPRequestTypes.h"
#include "Player/DSLocalPlayerSubsystem.h"

// QuickMatch 코드
void UGameSessionsManager::JoinGameSession()
{
	BroadcastGameSessionMessage.Broadcast(TEXT("Searching for Game Session..."), false);

	check(APIData);	// 유효성 검사
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();	// HTTP 요청 객체 생성

	// HTTP 응답 완료 시 호출할 콜백 바인딩
	Request->OnProcessRequestComplete().BindUObject(this, &UGameSessionsManager::FindOrCreateGameSession_Response);
	// 호출할 REST API의 URL 가져오기 (엔드포인트는 APIData에 저장된 게임 세션 찾기 URL)
	const FString APIUrl = APIData->GetAPIEndpoint(DedicatedServersTags::GameSessionsAPI::FindOrCreateGameSession);
	
	// 요청 세팅
	Request->SetURL(APIUrl);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->ProcessRequest();


	UDSLocalPlayerSubsystem* LocalPlayerSubsystem = GetDSLocalPlayerSubsystem();
	if (IsValid(LocalPlayerSubsystem))
	{
		Request->SetHeader(TEXT("Authorization"), LocalPlayerSubsystem->GetAuthResult().AccessToken);
	}
}

void UGameSessionsManager::FindOrCreateGameSession_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	// 요청 실패 시
	if (!bWasSuccessful)
	{
		BroadcastGameSessionMessage.Broadcast(HTTPStatusMessages::SomethingWentWrong, true);
	}

	// HTTP 응답을 JSON 객체로 변환
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		if (ContainsErrors(JsonObject))	// 응답 내부에 에러 정보가 있으면 처리
		{
			BroadcastGameSessionMessage.Broadcast(HTTPStatusMessages::SomethingWentWrong, true);
		}
		
		FDSGameSession GameSession;															// 람다에서 적용했던 것 처럼 GameSession 데이터가 여기 저장됨
		FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &GameSession);	// GameSession 응답 정보를 구조체로 파싱
		
		const FString GameSessionId = GameSession.GameSessionId;
		const FString GameSessionStatus = GameSession.Status;
		HandleGameSessionStatus(GameSessionStatus, GameSessionId);
	}
}

// Host GameSession 코드
void UGameSessionsManager::HostGameSession()
{
	BroadcastGameSessionMessage.Broadcast(TEXT("Creating a Game Session..."), false);

	check(APIData); // API 경로 정보 확인
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();

	Request->OnProcessRequestComplete().BindUObject(this, &UGameSessionsManager::HostGameSession_Response);

	const FString APIUrl = APIData->GetAPIEndpoint(DedicatedServersTags::GameSessionsAPI::CreateGameSession);
	Request->SetURL(APIUrl);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->ProcessRequest();

	/*UDSLocalPlayerSubsystem* LocalPlayerSubsystem = GetDSLocalPlayerSubsystem();
	if (IsValid(LocalPlayerSubsystem))
	{
		Request->SetHeader(TEXT("Authorization"), LocalPlayerSubsystem->GetAuthResult().AccessToken);

		// JSON 본문 구성
		TMap<FString, FString> Params = {
			{ TEXT("hostUsername"), LocalPlayerSubsystem->Username }, // 방장 이름
			{ TEXT("maxPlayers"), TEXT("2") } // 최대 인원
		};

		const FString Content = SerializeJsonContent(Params);
		Request->SetContentAsString(Content);
		Request->ProcessRequest();
	}*/
	UDSLocalPlayerSubsystem* LocalPlayerSubsystem = GetDSLocalPlayerSubsystem();
	if (IsValid(LocalPlayerSubsystem))
	{
		Request->SetHeader(TEXT("Authorization"), LocalPlayerSubsystem->GetAuthResult().AccessToken);
	}
}

void UGameSessionsManager::HostGameSession_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	// 요청 실패시
	if (!bWasSuccessful)
	{
		BroadcastGameSessionMessage.Broadcast(TEXT("Failed to create game session."), true);
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		if (ContainsErrors(JsonObject))
		{
			BroadcastGameSessionMessage.Broadcast(TEXT("Error in response while creating session."), true);
			return;
		}

		FDSGameSession GameSession;															// 람다에서 적용했던 것 처럼 GameSession 데이터가 여기 저장됨
		FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &GameSession);	// GameSession 응답 정보를 구조체로 파싱
		GameSession.Dump();
		
		const FString GameSessionId = GameSession.GameSessionId;

		if (GameSessionId.IsEmpty())
		{
			UE_LOG(LogTemp, Error, TEXT("GameSessionId is empty after GameSession creation!"));
		}
		else
		{
			BroadcastGameSessionMessage.Broadcast(TEXT("Create a Game Session. Creating a Player Session..."), false);

			if (UDSLocalPlayerSubsystem* DSLocalPlayerSubsystem = GetDSLocalPlayerSubsystem(); IsValid(DSLocalPlayerSubsystem))
			{
				TryCreatePlayerSession(DSLocalPlayerSubsystem->Username, GameSessionId);
			}
		}
		
		// 여기서 Widget에 띄워줄 데이터는 GameSessionId, Name, PlayerCount 등
		// 예시: "Alex's Room", "1/2"
		

		// OnGameSessionCreated.Broadcast(GameSession); // 사용자 정의 델리게이트로 위젯에 전달 / 아직 안쓰는 곳

		// ✅ 여기 추가: 방 만든 사람이 본인도 들어가도록 플레이어 세션 생성 시도
	}
}

FString UGameSessionsManager::GetUniquePlayerId() const
{
	APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());	// 월드에서 플레이어 컨트롤러 가져오기
	if (IsValid(LocalPlayerController))	// 유효성 검사
	{
		// 플레이어 컨트롤러에서 state 가져옴 -> 여기에 유저의 고유 ID가 들어 있음
		APlayerState* LocalPlayerState = LocalPlayerController->GetPlayerState<APlayerState>();
		if (IsValid(LocalPlayerState) && LocalPlayerState->GetUniqueId().IsValid())
		{
			// 고유 ID를 문자열로 변환 -> Player_ 붙여 반환
			return TEXT("Player_") + FString::FromInt(LocalPlayerState->GetUniqueID());
		}
	}
	
	return FString();	// 위 조건 만족 못하면 빈 문자열 반환
}


void UGameSessionsManager::HandleGameSessionStatus(const FString& Status, const FString& SessionId)
{
	if (Status.Equals(TEXT("ACTIVE")))		// GameLift에서 받은 GameSession이 이미 활성화된 상태인지 확인
	{
		// 활성화 된 상태라면 델리게이트 호출
		// 유저의 고유 ID와 세션 ID를 바탕으로 플레이어 세션을 생성 시도
		BroadcastGameSessionMessage.Broadcast(TEXT("Found active Game Session. Creating a Player Session..."), false);

		// LocalplayerSubsystem 유효성 검사 후
		if (UDSLocalPlayerSubsystem* DSLocalPlayerSubsystem = GetDSLocalPlayerSubsystem(); IsValid(DSLocalPlayerSubsystem))
		{
			// DSLocalPlayerSubsystem에 있는 Username과 SessionId를 사용해서 PlayerSession 만들기
			TryCreatePlayerSession(DSLocalPlayerSubsystem->Username, SessionId);
		}
	}
	else if (Status.Equals(TEXT("ACTIVATING")))		// GameSession을 초기화 중이라면
	{
		FTimerDelegate CreateSessionDelegate;
		CreateSessionDelegate.BindUObject(this, &ThisClass::JoinGameSession);
		APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
 		
		if (IsValid(LocalPlayerController))
		{
			// 0.5초 뒤에 재시도 (델리게이트)
			UE_LOG(LogTemp, Warning, TEXT("Call back JoinGameSession current Status : ACTIVATING"))
			LocalPlayerController->GetWorldTimerManager().SetTimer(CreateSessionTimer, CreateSessionDelegate, 0.5f, false);
		}
	}
	else	// 아무것도 아니면 에러 출력
	{
		BroadcastGameSessionMessage.Broadcast(HTTPStatusMessages::SomethingWentWrong, true);
	}
}
 
void UGameSessionsManager::TryCreatePlayerSession(const FString& PlayerId, const FString& GameSessionId)
{
	// GameSession에 접속이 가능하면 실행
	check(APIData);
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UGameSessionsManager::CreatePlayerSession_Response);
	const FString APIUrl = APIData->GetAPIEndpoint(DedicatedServersTags::GameSessionsAPI::CreatePlayerSession);
	
	Request->SetURL(APIUrl);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	// TMap 형식의 JSON 본문으로 넣을 키-값 쌍 준비
	TMap<FString, FString> Params = {
		{ TEXT("playerId"), PlayerId },				// 람다 함수에서 설정한 키 값
		{ TEXT("gameSessionId"), GameSessionId }		// ''
	};
	const FString Content = SerializeJsonContent(Params);	// 위 TMap을 JSON 문자열로 변환	 (HTTPRequestManager에 만든 함수)

	// 요청 전송
	Request->SetContentAsString(Content);
	Request->ProcessRequest();
}

// 방 생성 본체는 여기있음
void UGameSessionsManager::CreatePlayerSession_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful)	// 통신 실패시
	{
		BroadcastGameSessionMessage.Broadcast(HTTPStatusMessages::SomethingWentWrong, true);
	}
 
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))	// JSON 문자열 -> FJsonObject 파싱 시도
	{
		if (ContainsErrors(JsonObject))		// 응답 JSON 안에 errorType, errorMessage, $fault 등의 필드가 있는지 체크
		{
			BroadcastGameSessionMessage.Broadcast(HTTPStatusMessages::SomethingWentWrong, true);
		}

		// 받은 JSON 응답을 C++ 구조체로 변환
		FDSPlayerSession PlayerSession;
		FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &PlayerSession);
		PlayerSession.Dump();	// 출력해보기

		APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
		if (IsValid(LocalPlayerController))
		{
			FInputModeGameOnly InputModeData;
			LocalPlayerController->SetInputMode(InputModeData);		// 입력을 게임에만 적용되도록 설정
			LocalPlayerController->SetShowMouseCursor(false);		// 커서 비활성화
		}
		
		const FString Options = "?PlayerSessionId=" + PlayerSession.PlayerSessionId + "?Username=" + PlayerSession.PlayerId; // SessionID, PlayerID 가져오기
		const FString IpAndPort = PlayerSession.IpAddress + TEXT(":") + FString::FromInt(PlayerSession.Port);	// IP와 Port를 합쳐서 "127.0.0.1:7777" 같은 문자열 생성
		const FName Address(*IpAndPort);		// 위 IpAndPort를 FString -> FName 형변환
		UE_LOG(LogTemp, Warning, TEXT("Connecting to address: %s"), *IpAndPort);
		
		/* 클라이언트를 해당 Game Session 서버에 접속시키는 핵심 함수
		내부적으로 open 127.0.0.1:7777 콘솔 명령을 실행한 것과 같음
		이제 클라이언트는 해당 Dedicated Server와 연결돼서 멀티플레이 게임에 참가함
		Server Map을 프로젝트 세팅에서 설정해뒀기 때문에 서버에 접속하면 자동으로 그 맵으로 이동함
		option을 통해서 정보 제공(SessionID, PlayerID) */
		UGameplayStatics::OpenLevel(this, Address, true, Options); // world context, level Name, SessionID, PlayerID 넣기
	}
}