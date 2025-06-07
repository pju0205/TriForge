// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TFHUD.generated.h"

class UTFOverlay;

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()
public:
	UTexture2D* CrosshairsCenter;
	UTexture2D* CrosshairsLeft;
	UTexture2D* CrosshairsRight;
	UTexture2D* CrosshairsTop;
	UTexture2D* CrosshairsBottom;
	
};

/**
 * 
 */
UCLASS()
class TRIFORGE_API ATFHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

	UPROPERTY(EditAnywhere, Category = "PlayerState")
	TSubclassOf<UUserWidget> CharacterOverlayClass;
	
	UPROPERTY()
	UTFOverlay* CharacterOverlay;

	UFUNCTION()
	UTFOverlay* GetCharacterOverlay() const { return CharacterOverlay; }

	void AddCharacterOverlay();

protected:
	virtual void BeginPlay() override;
	
private:
	FHUDPackage HUDPackage;

	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter);
public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackage = Package; }
};
