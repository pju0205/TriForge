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
void UGameSessionsManager::QuickMatchGameSession()
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

	// 현재 로그인된 로컬 유저의 인증 토큰(AccessToken)을 HTTP 요청 헤더에 포함시키는 작업
	// 유효한 유저가 요청한 작업인가 체크
	/*UDSLocalPlayerSubsystem* LocalPlayerSubsystem = GetDSLocalPlayerSubsystem();
	if (IsValid(LocalPlayerSubsystem))
	{
		Request->SetHeader(TEXT("Authorization"), LocalPlayerSubsystem->GetAuthResult().AccessToken);
	}*/

	// API Gateway에서 Lambda 프록시 통합을 활성화해야 추가 정보도 포함시킨 Json 파일 보낼 수 있음
	UDSLocalPlayerSubsystem* LocalPlayerSubsystem = GetDSLocalPlayerSubsystem();
	if (IsValid(LocalPlayerSubsystem))
	{
		// 인증 헤더 설정 (Cognito 인증 토큰)
		Request->SetHeader(TEXT("Authorization"), LocalPlayerSubsystem->GetAuthResult().AccessToken);

		// SON 바디 생성 (username 포함)
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
		JsonObject->SetStringField(TEXT("username"), LocalPlayerSubsystem->Username);

		FString RequestBody;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
		FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

		// Content-Length 명시적으로 설정
		Request->SetHeader(TEXT("Content-Length"), FString::FromInt(FTCHARToUTF8(*RequestBody).Length()));  // UTF-8 기준 길이로 설정 권장

		Request->SetContentAsString(RequestBody);

		UE_LOG(LogTemp, Warning, TEXT("RequestBody: %s"), *RequestBody);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("LocalPlayerSubsystem is invalid"));
		return;
	}
	
	Request->ProcessRequest();
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
		
		TSharedPtr<FJsonObject> GameSessionJson = JsonObject->GetObjectField(TEXT("gameSession"));
		
		FDSGameSession GameSession;															// 람다에서 적용했던 것 처럼 GameSession 데이터가 여기 저장됨
		// FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &GameSession);	// GameSession 응답 정보를 구조체로 파싱
		if (!FJsonObjectConverter::JsonObjectToUStruct(GameSessionJson.ToSharedRef(), &GameSession))
		{
			UE_LOG(LogTemp, Error, TEXT("GameSession 변환 실패"));
			BroadcastGameSessionMessage.Broadcast(HTTPStatusMessages::SomethingWentWrong, true);
			return;
		}
		GameSession.Dump();
		
		const FString GameSessionId = GameSession.GameSessionId;
		const FString GameSessionStatus = GameSession.Status;

		UE_LOG(LogTemp, Warning, TEXT("Game Session Status: %s"), *GameSessionStatus);
		UE_LOG(LogTemp, Warning, TEXT("Game Session ID: %s"), *GameSessionId);
		HandleGameSessionStatus(GameSessionStatus, GameSessionId);
	}
}

// Host GameSession 코드
void UGameSessionsManager::HostGameSession()
{
	BroadcastGameSessionMessage.Broadcast(TEXT("Searching for Game Session..."), false);

	check(APIData);	// 유효성 검사
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();	// HTTP 요청 객체 생성

	// HTTP 응답 완료 시 호출할 콜백 바인딩
	Request->OnProcessRequestComplete().BindUObject(this, &UGameSessionsManager::HostGameSession_Response);
	// 호출할 REST API의 URL 가져오기 (엔드포인트는 APIData에 저장된 게임 세션 찾기 URL)
	const FString APIUrl = APIData->GetAPIEndpoint(DedicatedServersTags::GameSessionsAPI::CreateGameSession);
	
	// 요청 세팅
	Request->SetURL(APIUrl);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	// API Gateway에서 Lambda 프록시 통합을 활성화해야 추가 정보도 포함시킨 Json 파일 보낼 수 있음
	UDSLocalPlayerSubsystem* LocalPlayerSubsystem = GetDSLocalPlayerSubsystem();
	if (IsValid(LocalPlayerSubsystem))
	{
		// 인증 헤더 설정 (Cognito 인증 토큰)
		Request->SetHeader(TEXT("Authorization"), LocalPlayerSubsystem->GetAuthResult().AccessToken);

		// SON 바디 생성 (username 포함)
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
		JsonObject->SetStringField(TEXT("username"), LocalPlayerSubsystem->Username);

		FString RequestBody;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
		FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

		// Content-Length 명시적으로 설정
		Request->SetHeader(TEXT("Content-Length"), FString::FromInt(FTCHARToUTF8(*RequestBody).Length()));  // UTF-8 기준 길이로 설정 권장

		Request->SetContentAsString(RequestBody);

		UE_LOG(LogTemp, Warning, TEXT("RequestBody: %s"), *RequestBody);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("LocalPlayerSubsystem is invalid"));
		return;
	}

	Request->ProcessRequest();
}

void UGameSessionsManager::HostGameSession_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("HostGameSession Response: %s"), *Response->GetContentAsString());
	// 요청 실패 시
	if (!bWasSuccessful)
	{
		BroadcastGameSessionMessage.Broadcast(HTTPStatusMessages::SomethingWentWrong, true);
	}

	// HTTP 응답을 JSON 객체로 변환
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	// JSON 파싱 성공 후 (새로운 구조)
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		if (ContainsErrors(JsonObject)) {
			BroadcastGameSessionMessage.Broadcast(HTTPStatusMessages::SomethingWentWrong, true);
		}
    
		// 1) "gameSession" 필드만 추출 ★
		TSharedPtr<FJsonObject> GameSessionJson = JsonObject->GetObjectField(TEXT("gameSession"));

		// 2) gameSession 오브젝트만 FDSGameSession 구조체에 변환
		FDSGameSession GameSession;
		if (!FJsonObjectConverter::JsonObjectToUStruct(GameSessionJson.ToSharedRef(), &GameSession))
		{
			UE_LOG(LogTemp, Error, TEXT("GameSession 변환 실패"));
			BroadcastGameSessionMessage.Broadcast(HTTPStatusMessages::SomethingWentWrong, true);
			return;
		}
		GameSession.Dump();

		const FString GameSessionId = GameSession.GameSessionId;

		if (UDSLocalPlayerSubsystem* DSLocalPlayerSubsystem = GetDSLocalPlayerSubsystem(); IsValid(DSLocalPlayerSubsystem))
		{
			TryCreatePlayerSession(DSLocalPlayerSubsystem->Username, GameSessionId);
		}
	}

	// 기존 구조
	/*if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		if (ContainsErrors(JsonObject))	// 응답 내부에 에러 정보가 있으면 처리
		{
			BroadcastGameSessionMessage.Broadcast(HTTPStatusMessages::SomethingWentWrong, true);
		}
		
		FDSGameSession GameSession;															// 람다에서 적용했던 것 처럼 GameSession 데이터가 여기 저장됨
		FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &GameSession);	// GameSession 응답 정보를 구조체로 파싱
		GameSession.Dump();
		
		const FString GameSessionId = GameSession.GameSessionId;
		UE_LOG(LogTemp, Warning, TEXT("Creator: %s"), *GameSession.CreatorId);
		
		if (UDSLocalPlayerSubsystem* DSLocalPlayerSubsystem = GetDSLocalPlayerSubsystem(); IsValid(DSLocalPlayerSubsystem))
		{
			TryCreatePlayerSession(DSLocalPlayerSubsystem->Username, GameSessionId);
		}
	}*/
}

void UGameSessionsManager::RetrieveGameSessions()
{
	BroadcastGameSessionMessage.Broadcast(TEXT("Retrieving for Game Session..."), false);

	check(APIData);	// 유효성 검사
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();	// HTTP 요청 객체 생성

	// HTTP 응답 완료 시 호출할 콜백 바인딩
	Request->OnProcessRequestComplete().BindUObject(this, &UGameSessionsManager::RetrieveGameSession_Response);
	// 호출할 REST API의 URL 가져오기 (엔드포인트는 APIData에 저장된 게임 세션 찾기 URL)
	const FString APIUrl = APIData->GetAPIEndpoint(DedicatedServersTags::GameSessionsAPI::RetrieveGameSession);
	
	// 요청 세팅
	Request->SetURL(APIUrl);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	
	// 현재 로그인된 로컬 유저의 인증 토큰(AccessToken)을 HTTP 요청 헤더에 포함시키는 작업
	UDSLocalPlayerSubsystem* LocalPlayerSubsystem = GetDSLocalPlayerSubsystem();
	if (IsValid(LocalPlayerSubsystem))
	{
		Request->SetHeader(TEXT("Authorization"), LocalPlayerSubsystem->GetAuthResult().AccessToken);
	}
	
	Request->ProcessRequest();
}

void UGameSessionsManager::RetrieveGameSession_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	// 요청 실패 시
	if (!bWasSuccessful)
	{
		BroadcastGameSessionMessage.Broadcast(HTTPStatusMessages::SomethingWentWrong, true);
	}

	// HTTP 응답을 JSON 객체로 변환
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	FDSGameSessionResponse ResponseStruct;
	
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		if (ContainsErrors(JsonObject))
		{
			BroadcastGameSessionMessage.Broadcast(HTTPStatusMessages::SomethingWentWrong, true);
			return;
		}

		// 전체 응답 구조체로 변환 ResponseStruct
		FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &ResponseStruct);

		
		// 활성화된 방 없으면
		if (ResponseStruct.sessions.Num() == 0)
		{
			BroadcastGameSessionMessage.Broadcast(TEXT("No active game sessions found."), true);
			return;
		}

		BroadcastGameSessionMessage.Broadcast(TEXT("Adding game session list.."), false);
		
		// 배열 처리
		for (const FDSGameSession& GameSession : ResponseStruct.sessions)
		{
			OnRetrieveGameSession.Broadcast(GameSession); // 여러 개를 개별적으로 위젯에 전달 가능
		}

		BroadcastGameSessionMessage.Broadcast(TEXT("Successfully added Game Session list"), true);
	}
	else
	{
		BroadcastGameSessionMessage.Broadcast(HTTPStatusMessages::SomethingWentWrong, true);
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
		CreateSessionDelegate.BindUObject(this, &ThisClass::QuickMatchGameSession);
		APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
 		
		if (IsValid(LocalPlayerController))
		{
			// 0.5초 뒤에 재시도 (델리게이트)
			UE_LOG(LogTemp, Warning, TEXT("Call back JoinGameSession current Status : ACTIVATING"));
			LocalPlayerController->GetWorldTimerManager().SetTimer(CreateSessionTimer, CreateSessionDelegate, 0.5f, false);
		}
	}
	else	// 아무것도 아니면 에러 출력
	{
		UE_LOG(LogTemp, Warning, TEXT("Call back JoinGameSession current Status : nullptr"));
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