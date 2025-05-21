// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/TFHUD.h"

#include "Blueprint/UserWidget.h"
#include "HUD/TFOverlay.h"

void ATFHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

		if (HUDPackage.CrosshairsCenter)
		{
			DrawCrosshair(HUDPackage.CrosshairsCenter, ViewportCenter);
		}
		if (HUDPackage.CrosshairsLeft)
		{
			DrawCrosshair(HUDPackage.CrosshairsLeft, ViewportCenter);
		}
		if (HUDPackage.CrosshairsRight)
		{
			DrawCrosshair(HUDPackage.CrosshairsRight, ViewportCenter);
		}
		if (HUDPackage.CrosshairsTop)
		{
			DrawCrosshair(HUDPackage.CrosshairsTop, ViewportCenter);
		}
		if (HUDPackage.CrosshairsBottom)
		{
			DrawCrosshair(HUDPackage.CrosshairsBottom, ViewportCenter);
		}
	}
}

void ATFHUD::SetHealthBar(float Health, float MaxHealth)
{
	if (CharacterOverlay == nullptr) return;

	CharacterOverlay->UpdateHealthBar(Health, MaxHealth);
}

void ATFHUD::BeginPlay()
{
	Super::BeginPlay();

	AddCharacterOverlay();
}

void ATFHUD::AddCharacterOverlay()
{
	APlayerController* PlayerController =  GetOwningPlayerController();
	if (PlayerController && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UTFOverlay>(PlayerController, CharacterOverlayClass);
		
		CharacterOverlay->AddToViewport();
	}
}

void ATFHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter)
{
	// 정확한 Null Check를 위해 추가 (Map 이동시 Null 출력으로 충돌 일어남)
	if (!Texture) return;
	
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D TextureDrawPoint(
		ViewportCenter.X - (TextureWidth / 2.f),
		ViewportCenter.Y - (TextureHeight / 2.f)
	);

	DrawTexture(
		Texture,
		TextureDrawPoint.X,
		TextureDrawPoint.Y,
		TextureWidth,
		TextureHeight,
		0.f,
		0.f,
		1.f,
		1.f,
		FLinearColor::White
	);
}
