
#pragma once

#include "CoreMinimal.h"
#include "PlayerState/TFMatchPlayerState.h"
#include "TFPlayerState.generated.h"

class ATFPlayerCharacter;
class ATFPlayerController;
/**
 * 
 */
UCLASS()
class TRIFORGE_API ATFPlayerState : public ATFMatchPlayerState
{
	GENERATED_BODY()

public:
	ATFPlayerState();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	UPROPERTY()
	ATFPlayerController* TFPlayerController;

	UPROPERTY()
	ATFPlayerCharacter* TFPlayerCharacter;
	
	// 체력 관련 HealthComponent -> TFplayerController에 바로 HUD 반영하도록 변경했음
};
