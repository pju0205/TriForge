// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Portal/PortalHUD.h"

#include "UI/Portal/SignIn/SignInOverlay.h"
#include "Blueprint/UserWidget.h"
#include "UI/Portal/Dashboard/DashboardOverlay.h"

// 첫 화면 띄우기 (로그인 화면)
void APortalHUD::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* OwningPlayerController = GetOwningPlayerController();
 	
	SignInOverlay = CreateWidget<USignInOverlay>(OwningPlayerController, SignInOverlayClass);
	if (IsValid(SignInOverlay))
	{
		SignInOverlay->AddToViewport();	// 뷰에 띄우기
	}

	// 그냥 위젯만 다루기 때문에 player(Pawn) 움직이지 않는 입력 모드 선언
	FInputModeGameAndUI InputModeData;
	OwningPlayerController->SetInputMode(InputModeData);	// 소유 컨트롤러 입력모드로 설정
	OwningPlayerController->SetShowMouseCursor(true);		// 마우스 커서 움직이도록 설정
}

// 로그인 완료 시 화면 변경 (Dashboard 화면으로)
void APortalHUD::OnSignIn()
{
	if (IsValid(SignInOverlay))
	{
		SignInOverlay->RemoveFromParent();		// 로그인 화면 지우기
	}
	
	APlayerController* OwningPlayerController = GetOwningPlayerController();
	DashboardOverlay = CreateWidget<UDashboardOverlay>(OwningPlayerController, DashboardOverlayClass);
	if (IsValid(DashboardOverlay))
	{
		DashboardOverlay->AddToViewport();		// Dashboard 화면 띄우기
	}
}


// 로그아웃 완료 시 화면 변경 (로그인 화면으로)
void APortalHUD::OnSignOut()
{
	if (IsValid(DashboardOverlay))
	{
		DashboardOverlay->RemoveFromParent();		// Dashboard 화면 지우기
	}
	
	APlayerController* OwningPlayerController = GetOwningPlayerController();
	SignInOverlay = CreateWidget<USignInOverlay>(OwningPlayerController, SignInOverlayClass);
	if (IsValid(SignInOverlay))
	{
		SignInOverlay->AddToViewport();				// 로그인 화면 띄우기
	}
}