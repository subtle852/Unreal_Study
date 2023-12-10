// AN_SequenceEnded.h

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_SequenceEnded.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPROJECT_API UAN_SequenceEnded : public UAnimNotify
{
	GENERATED_BODY()
	
private:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference);

};
