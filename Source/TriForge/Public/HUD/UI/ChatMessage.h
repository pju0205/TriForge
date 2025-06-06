﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatMessage.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class TRIFORGE_API UChatMessage : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_Message;
};
