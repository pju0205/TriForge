// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PortalManagement.generated.h"

// Portal의 기능을 관리하기 위해 만든 인터페이스
// This class does not need to be modified.
UINTERFACE()
class UPortalManagement : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DEDICATEDSERVERS_API IPortalManagement
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void RefreshTokens(const FString& RefreshToken) = 0;
};
