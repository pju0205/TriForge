#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TFCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UTFAnimInstance;

UCLASS()
class TRIFORGE_API ATFCharacter : public ACharacter
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;

public:

	ATFCharacter();
};
