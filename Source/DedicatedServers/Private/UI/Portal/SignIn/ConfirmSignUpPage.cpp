// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Portal/SignIn/ConfirmSignUpPage.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"


 void UConfirmSignUpPage::ClearTextBoxes()
 {
 	TextBox_ConfirmationCode->SetText(FText::GetEmpty());
 	TextBlock_StatusMessage->SetText(FText::GetEmpty());
 	TextBlock_Destination->SetText(FText::GetEmpty());
 }
 
void UConfirmSignUpPage::UpdateStatusMessage(const FString& Message, bool bShouldResetWidgets)
 {
 	TextBlock_StatusMessage->SetText(FText::FromString(Message));
 	if (bShouldResetWidgets)
 	{
 		Button_Confirm->SetIsEnabled(true);
 	}
 }

void UConfirmSignUpPage::NativeConstruct()
 {
 	Super::NativeConstruct();

 	// 텍스트 변경 때 마다 버튼 상태 변경
 	TextBox_ConfirmationCode->OnTextChanged.AddDynamic(this, &UConfirmSignUpPage::UpdateConfirmButtonState);
 	Button_Confirm->SetIsEnabled(false);	// 처음에는 Confirm 버튼을 비활성화 상태
 }
 
void UConfirmSignUpPage::UpdateConfirmButtonState(const FText& Text)
 {
 	// 정규식 패턴: 정확히 6자리 숫자인 경우만 통과
 	const FRegexPattern SixDigitsPattern(TEXT(R"(^\d{6}$)"));
 	FRegexMatcher Matcher(SixDigitsPattern, Text.ToString());

 	// 정규식 일치 확인 
 	const bool bValidConfirmationCode = Matcher.FindNext();

 	// 정규식에 맞으면 버튼 활성화, 아니면 비활성화
 	Button_Confirm->SetIsEnabled(bValidConfirmationCode);

 	// 상태 메시지를 업데이트 
 	if (bValidConfirmationCode)
 	{
 		TextBlock_StatusMessage->SetText(FText::FromString(""));	// 유효하면 빈 문자열
 	}
 	else
 	{
 		TextBlock_StatusMessage->SetText(FText::FromString("Please enter six numerical digits.")); // 유효하지 않으면 출력
 	}
 }