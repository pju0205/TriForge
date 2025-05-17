
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "TFMeleeANS.generated.h"

/**
 * 
 */
UCLASS()
class TRIFORGE_API UTFMeleeANS : public UAnimNotifyState
{
	GENERATED_BODY()

protected:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
