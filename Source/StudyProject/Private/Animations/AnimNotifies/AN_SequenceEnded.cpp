// AN_SequenceEnded.cpp


#include "Animations/AnimNotifies/AN_SequenceEnded.h"
#include "Characters/SRPGCharacter.h"

void UAN_SequenceEnded::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    if (true == ::IsValid(MeshComp))
    {
        ASRPGCharacter* HitReactingCharacter = Cast<ASRPGCharacter>(MeshComp->GetOwner());
        if (true == ::IsValid(HitReactingCharacter))
        {
            HitReactingCharacter->SequenceEnded();
        }
    }
}