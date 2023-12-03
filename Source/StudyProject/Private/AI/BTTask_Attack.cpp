// BTTask_Attack.cpp


#include "AI/BTTask_Attack.h"
#include "Controllers/SAIController.h"
#include "Characters/SNonPlayerCharacter.h"
#include "Characters/STestCharacter.h"

UBTTask_Attack::UBTTask_Attack()
{
    bNotifyTick = true;
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    ASAIController* AIC = Cast<ASAIController>(OwnerComp.GetAIOwner());
    if (true == ::IsValid(AIC))
    {
        ASTestCharacter* NPC = Cast<ASTestCharacter>(AIC->GetPawn());
        if (true == ::IsValid(NPC))
        {
            if (false == NPC->IsNowAttacking())
            {
                FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
            }
        }
    }
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

    ASAIController* AIC = Cast<ASAIController>(OwnerComp.GetAIOwner());
    if (true == ::IsValid(AIC))
    {
        ASTestCharacter* NPC = Cast<ASTestCharacter>(AIC->GetPawn());
        if (true == ::IsValid(NPC))
        {
            NPC->Attack();
        }
    }

    return EBTNodeResult::InProgress;
}