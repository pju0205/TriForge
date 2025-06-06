// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Chatting.generated.h"

class UChatMessage;
class UEditableTextBox;
class UScrollBox;
/**
 * 
 */
UCLASS()
class TRIFORGE_API UChatting : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	UScrollBox* ScrollBox_Chat;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* EditableTextBox_Chat;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chat")
	TSubclassOf<UChatMessage> ChatMessageWidgetClass;

	UFUNCTION()
	void OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	
	UFUNCTION()
	void AddChatMessage(const FString& Message);

	void ActivateChatText();
	void DeactiveChatText(APlayerController* PlayerController);

	UFUNCTION()
	void OnPawnChanged(APawn* OldPawn, APawn* NewPawn);

	void ToggleChatVisibility();
};
