// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SignInOverlay.generated.h"


class UPortalManager;
class UQuickMatchGame;
class UWidgetSwitcher;
class USignInPage;
class USignUpPage;
class UConfirmSignUpPage;
class USuccessConfirmedPage;
class UButton;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API USignInOverlay : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPortalManager> PortalManagerClass;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher;

protected:
	// 처음 초기화
	virtual void NativeConstruct() override;
	
private:
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USignInPage> SignInPage;
 
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USignUpPage> SignUpPage;
 	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UConfirmSignUpPage> ConfirmSignUpPage;
 
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USuccessConfirmedPage> SuccessConfirmedPage;
	
	UPROPERTY()
	TObjectPtr<UPortalManager> PortalManager;

	void AutoSignIn();
	
	// Show Sign Page
	UFUNCTION()
	void ShowSignInPage();
 
	UFUNCTION()
	void ShowSignUpPage();
 
	UFUNCTION()
	void ShowConfirmSignUpPage();
 
	UFUNCTION()
	void ShowSuccessConfirmedPage();

	// Sign Page Button
	UFUNCTION()
	void SignInButtonClicked();
 
	UFUNCTION()
	void SignUpButtonClicked();
 
	UFUNCTION()
	void ConfirmButtonClicked();

	// Sign Up Succeeded
	UFUNCTION()
	void OnSignUpSucceeded();
 
	UFUNCTION()
	void OnConfirmSucceeded();
};