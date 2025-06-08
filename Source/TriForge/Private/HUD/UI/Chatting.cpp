// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/UI/Chatting.h"

#include "Character/TFPlayerController.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Game/DSGameState.h"
#include "Game/DSLobbyGameMode.h"
#include "HUD/UI/ChatMessage.h"


void UChatting::NativeConstruct()
{
	Super::NativeConstruct();

	// Pawn 변경됐을 때 대비
	if (ATFPlayerController* TFPC = Cast<ATFPlayerController>(GetOwningPlayer()))
	{
		TFPC->OnPossessedPawnChanged.AddDynamic(this, &UChatting::OnPawnChanged);
	}

	// 처음에 초기화 시도
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		EditableTextBox_Chat->OnTextCommitted.AddDynamic(this, &UChatting::OnTextCommitted);
		EditableTextBox_Chat->SetIsEnabled(false);
	}
}

void UChatting::NativeDestruct()
{
	Super::NativeDestruct();

	// Unbind all delegates to avoid duplicates
	if (EditableTextBox_Chat)
	{
		EditableTextBox_Chat->OnTextCommitted.RemoveAll(this);
	}
}

void UChatting::OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	// OnEnter == 뭐가가 입력됨
	if (CommitMethod == ETextCommit::OnEnter && EditableTextBox_Chat)
	{ 
		FText InputText = EditableTextBox_Chat->GetText();

		if (!InputText.IsEmpty())
		{
			ATFPlayerController* PlayerController = Cast<ATFPlayerController>(GetWorld()->GetFirstPlayerController());
			if (PlayerController)
			{
				FString Message = FString::Printf(TEXT("%s: %s"), *PlayerController->Username, *InputText.ToString());

				// 채팅 메시지를 보내기 위한 Server RPC 호출
				PlayerController->ServerSendChatMessage(Message);
                
				//메시지를 뿌리고 나서는 채팅창 비활성화
				DeactiveChatText(PlayerController);
			}
		}
		else
		{
			ATFPlayerController* PlayerController = Cast<ATFPlayerController>(GetWorld()->GetFirstPlayerController());
			if (PlayerController)
			{
				//메시지를 뿌리고 나서는 채팅창 비활성화
				DeactiveChatText(PlayerController);
			}
			
		}
	}
}

// 채팅창 활성화
void UChatting::ActivateChatText()
{
	if (EditableTextBox_Chat)
	{
		EditableTextBox_Chat->SetIsEnabled(true);
		EditableTextBox_Chat->SetFocus();
	}
}

// 채팅창 비활성화
void UChatting::DeactiveChatText(APlayerController* PlayerController)
{
	if (EditableTextBox_Chat && PlayerController)
	{
		EditableTextBox_Chat->SetText(FText::GetEmpty());
		EditableTextBox_Chat->SetIsEnabled(false);

		ADSPlayerController* DSPlayerController = Cast<ADSPlayerController>(PlayerController);
		if (DSPlayerController)
		{
			AGameStateBase* GameState = DSPlayerController->GetWorld()->GetGameState();
			if (GameState && GameState->IsA(ADSGameState::StaticClass()))
			{
				// 로비 모드로 되돌림
				DSPlayerController->Client_SetToLobbyMode();
				UE_LOG(LogTemp, Warning, TEXT("Call LobbyInputMode"));
				return;
			}
			else
			{
				FInputModeGameOnly InputMode;
				PlayerController->SetInputMode(InputMode);
				UE_LOG(LogTemp, Warning, TEXT("Call GameInputMode"));
				return;
			}
		}
	}
}

void UChatting::ToggleChatVisibility()
{
	ESlateVisibility Current = GetVisibility();

	if (Current == ESlateVisibility::Collapsed || Current == ESlateVisibility::Hidden)
	{
		SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UChatting::AddChatMessage(const FString& Message)
{
	if (!ScrollBox_Chat) return;

	UChatMessage* ChatMessage = CreateWidget<UChatMessage>(this, ChatMessageWidgetClass);
	if (ChatMessage && ChatMessage->TextBlock_Message)
	{
		ChatMessage->TextBlock_Message->SetText(FText::FromString(Message));
		ScrollBox_Chat->AddChild(ChatMessage);
		ScrollBox_Chat->ScrollToEnd();
	}
}

void UChatting::OnPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	UE_LOG(LogTemp, Log, TEXT("[UChatting] OnPawnChanged 호출됨."));

	// 새로 바인딩 필요할 수 있으므로, 기존 설정 재시도
	if (EditableTextBox_Chat)
	{
		EditableTextBox_Chat->OnTextCommitted.RemoveAll(this);
		EditableTextBox_Chat->OnTextCommitted.AddDynamic(this, &UChatting::OnTextCommitted);

		EditableTextBox_Chat->SetIsEnabled(false);
		EditableTextBox_Chat->SetText(FText::GetEmpty());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[UChatting] EditableTextBox_Chat가 nullptr입니다."));
	}

	// 필요 시 ChatPanel 다시 Visible 설정
	if (GetVisibility() != ESlateVisibility::Visible)
	{
		SetVisibility(ESlateVisibility::Visible);
	}

	UE_LOG(LogTemp, Log, TEXT("[UChatting] 채팅 위젯 입력 및 바인딩 재설정 완료."));
}
