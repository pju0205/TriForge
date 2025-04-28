// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HTTP/HTTPRequestManager.h"
#include "JsonObjectConverter.h"
#include "DedicatedServers/DedicatedServers.h"
#include "UI/HTTP/HTTPRequestTypes.h"
/*#include "Player/DSLocalPlayerSubsystem.h"*/


// Player 정보 관리 함수
/*UDSLocalPlayerSubsystem* UHTTPRequestManager::GetDSLocalPlayerSubsystem() const
{
 	APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
 	if (IsValid(LocalPlayerController))
 	{
 		ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(LocalPlayerController->Player);
 		if (IsValid(LocalPlayer))
 		{
 			return LocalPlayer->GetSubsystem<UDSLocalPlayerSubsystem>();
 		}
 	}
 	return nullptr;
}*/


// Manager 관련해서 에러 떳을 때 처리하는 함수
bool UHTTPRequestManager::ContainsErrors(TSharedPtr<FJsonObject> JsonObject)
{
 	if (JsonObject->HasField(TEXT("errorType")) || JsonObject->HasField(TEXT("errorMessage")))
 	{
 		FString ErrorType = JsonObject->HasField(TEXT("errorType")) ? JsonObject->GetStringField(TEXT("errorType")) : TEXT("Unknown Error");
 		FString ErrorMessage = JsonObject->HasField(TEXT("errorMessage")) ? JsonObject->GetStringField(TEXT("errorMessage")) : TEXT("Unknown Error Message");
 
 		UE_LOG(LogDedicatedServers, Error, TEXT("Error Type: %s"), *ErrorType);
 		UE_LOG(LogDedicatedServers, Error, TEXT("Error Message: %s"), *ErrorMessage);
 
 		return true;
 	}
 	if (JsonObject->HasField(TEXT("$fault")))
 	{
 		FString ErrorType = JsonObject->HasField(TEXT("name")) ? JsonObject->GetStringField(TEXT("name")) : TEXT("Unknown Error");
 		UE_LOG(LogDedicatedServers, Error, TEXT("Error Type: %s"), *ErrorType);
 		return true;
 	}
 	return false;
}


// Meta Data 관련 출력 함수
void UHTTPRequestManager::DumpMetaData(TSharedPtr<FJsonObject> JsonObject)
{
 	if (JsonObject->HasField(TEXT("$metadata")))
 	{
 		TSharedPtr<FJsonObject> MetaDataJsonObject = JsonObject->GetObjectField(TEXT("$metadata"));
 		FDSMetaData DSMetaData;
 		FJsonObjectConverter::JsonObjectToUStruct(MetaDataJsonObject.ToSharedRef(), &DSMetaData);
 		DSMetaData.Dump();
 	}
}


// TMap<FString, FString> 형식의 키-값 쌍을 JSON 문자열로 변환해주는 함수
// player Session 만들 때 사용함
FString UHTTPRequestManager::SerializeJsonContent(const TMap<FString, FString>& Params)
{
 	TSharedPtr<FJsonObject> ContentJsonObject = MakeShareable(new FJsonObject);

 	// 전달받은 키-값 쌍(Params)을 JSON 오브젝트에 추가
 	for (const auto& Param : Params)
 	{
 		ContentJsonObject->SetStringField(Param.Key, Param.Value);
 	}

 	// 최종 JSON 문자열로 변환
 	FString Content;
 	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&Content);
 	FJsonSerializer::Serialize(ContentJsonObject.ToSharedRef(), JsonWriter);
 	
 	return Content;
}
