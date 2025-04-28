// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Portal/SignIn/SignUpPage.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"


void USignUpPage::UpdateStatusMessage(const FString& Message, bool bShouldResetWidgets)
{
	TextBlock_StatusMessage->SetText(FText::FromString(Message));
	if (bShouldResetWidgets)
	{
		Button_SignUp->SetIsEnabled(true);
	}
}

void USignUpPage::ClearTextBoxes()
{
	TextBox_UserName->SetText(FText::GetEmpty());
	TextBox_Password->SetText(FText::GetEmpty());
	TextBox_ConfirmPassword->SetText(FText::GetEmpty());
	TextBox_Email->SetText(FText::GetEmpty());
}

void USignUpPage::NativeConstruct()
{
 	Super::NativeConstruct();


 	// 입력 값 동적으로 확인하여 상태 메시지 출력 & 버튼 활성화 여부 결정하기
 	TextBox_UserName->OnTextChanged.AddDynamic(this, &USignUpPage::UpdateSignUpButtonState);
 	TextBox_Password->OnTextChanged.AddDynamic(this, &USignUpPage::UpdateSignUpButtonState);
 	TextBox_ConfirmPassword->OnTextChanged.AddDynamic(this, &USignUpPage::UpdateSignUpButtonState);
 	TextBox_Email->OnTextChanged.AddDynamic(this, &USignUpPage::UpdateSignUpButtonState);

 	// 처음에는 회원가입 버튼 비활성화
 	Button_SignUp->SetIsEnabled(false);
}
 
void USignUpPage::UpdateSignUpButtonState(const FText& Text)
{
 	// 아이디, 패스워드 상태 확인 (유효성 검사)
 	const bool bIsUsernameValid = !TextBox_UserName->GetText().ToString().IsEmpty();		// 빈 공간이 아니라면
 	const bool bArePasswordsEqual = TextBox_Password->GetText().ToString() == TextBox_ConfirmPassword->GetText().ToString();
 	const bool bIsValiEmail = IsValidEmail(TextBox_Email->GetText().ToString());
 	const bool bIsPasswordLongEnough = TextBox_Password->GetText().ToString().Len() >= 8;

 	
 	FString StatusMessage;
 	const bool bIsStrongPassword = IsStrongPassword(TextBox_Password->GetText().ToString(), StatusMessage);

 	// 조건에 따라 상태 메시지 출력
 	if (!bIsStrongPassword)
 	{
 		TextBlock_StatusMessage->SetText(FText::FromString(StatusMessage));
 	}
 	else if (!bIsUsernameValid)	// User Name이 유효하지 않을 때
 	{
 		TextBlock_StatusMessage->SetText(FText::FromString(TEXT("Please enter a valid Username.")));
 	}
 	else if (!bArePasswordsEqual)	// Password가 일치 하지 않을 때
 	{
 		TextBlock_StatusMessage->SetText(FText::FromString(TEXT("Please ensure that passwords match.")));
 	}
 	else if (!bIsValiEmail)		// Email이 유효하지 않을 때
 	{
 		TextBlock_StatusMessage->SetText(FText::FromString(TEXT("Please enter a valid email.")));
 	}
 	else if (!bIsPasswordLongEnough)	// Password가 정해진 규칙을 따르지 않을 때
 	{
 		TextBlock_StatusMessage->SetText(FText::FromString(TEXT("Passwords must be at least 8 characters.")));
 	}
 	else						// 싹 다 아니면 상태 메세지 비우기
 	{
 		TextBlock_StatusMessage->SetText(FText::GetEmpty());
 	}

 	// 각 조건 다 충족해야 회원가입 버튼 활성화
 	Button_SignUp->SetIsEnabled(bIsUsernameValid && bArePasswordsEqual && bIsValiEmail && bIsStrongPassword);
}
 
bool USignUpPage::IsValidEmail(const FString& Email)
{
 	// 정규 표현식으로 이메일 포맷 검사
 	const FRegexPattern EmailPattern(TEXT(R"((^[^\s@]+@[^\s@]+\.[^\s@]{2,}$))"));
 	FRegexMatcher Matcher(EmailPattern, Email);
 	return Matcher.FindNext();
}
 
bool USignUpPage::IsStrongPassword(const FString& Password, FString& StatusMessage)
{

 	// 비밀번호에 숫자, 특수문자, 대문자, 소문자 포함 여부 확인
 	const FRegexPattern NumberPattern(TEXT(R"(\d)")); // contains at least one number
 	const FRegexPattern SpecialCharPattern(TEXT(R"([^\w\s])")); // contains at least one special character
 	const FRegexPattern UppercasePattern(TEXT(R"([A-Z])")); // contains at least one uppercase character
 	const FRegexPattern LowercasePattern(TEXT(R"([a-z])")); // contains at least one lowercase character
 
 	FRegexMatcher NumberMatcher(NumberPattern, Password);
 	FRegexMatcher SpecialCharMatcher(SpecialCharPattern, Password);
 	FRegexMatcher UppercaseMatcher(UppercasePattern, Password);
 	FRegexMatcher LowercaseMatcher(LowercasePattern, Password);
 
 	if (!NumberMatcher.FindNext())
 	{
 		StatusMessage = TEXT("Password must contain at least 1 number.");
 		return false;
 	}
 	if (!SpecialCharMatcher.FindNext())
 	{
 		StatusMessage = TEXT("Password must contain at least one special character.");
 		return false;
 	}
 	if (!UppercaseMatcher.FindNext())
 	{
 		StatusMessage = TEXT("Password must contain at least one uppercase character.");
 		return false;
 	}
 	if (!LowercaseMatcher.FindNext())
 	{
 		StatusMessage = TEXT("Password must contain at least one lowercase character.");
 		return false;
 	}

 	
 	return true;		// 모든 조건을 통과했을 경우 strong password로 판단
}