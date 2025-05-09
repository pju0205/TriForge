// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SignUpPage.generated.h"

class UTextBlock;
class UButton;
class UEditableTextBox;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API USignUpPage : public UUserWidget
{
	GENERATED_BODY()

public:
 
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> TextBox_UserName;
 
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> TextBox_Password;
 
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> TextBox_ConfirmPassword;
 
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> TextBox_Email;
 
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_SignUp;
 
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Back;
 
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_StatusMessage;

	UFUNCTION()
	void UpdateStatusMessage(const FString& Message, bool bShouldResetWidgets);

	void ClearTextBoxes();
	
	// 여기 부터는 선택사항 
	// 강한 보안 비밀번호 조건 만들기  (영어 소문자, 대문자, 특문 등등 조건)
	// FRegexMatcher, FRegexPattern 이라는걸 써서 비밀번호 설정
protected:
	virtual void NativeConstruct() override;
 
private:
 
	UFUNCTION()
	void UpdateSignUpButtonState(const FText& Text);		// 상태 메시지 출력 함수
 
	bool IsValidEmail(const FString& Email);									// 비밀번호 조건
	bool IsStrongPassword(const FString& Password, FString& StatusMessage);		// 이메일 조건
};
