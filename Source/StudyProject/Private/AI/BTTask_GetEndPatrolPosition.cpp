// BTTask_GetEndPatrolPosition.cpp


#include "AI/BTTask_GetEndPatrolPosition.h"
#include "Controllers/SAIController.h"
#include "Characters/SNonPlayerCharacter.h"
#include "Characters/STestCharacter.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_GetEndPatrolPosition::UBTTask_GetEndPatrolPosition()
{
    NodeName = TEXT("GetEndPatrolPosition"); // Behavior Tree�� ���� ��� �̸�.
}

EBTNodeResult::Type UBTTask_GetEndPatrolPosition::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

    if (EBTNodeResult::Failed == Result)
    {
        return Result;
    }

    ASAIController* AIController = Cast<ASAIController>(OwnerComp.GetAIOwner());
    if (false == ::IsValid(AIController))
    {
        return Result = EBTNodeResult::Failed;
    }

    ASTestCharacter* NPC = Cast<ASTestCharacter>(AIController->GetPawn());
    if (false == ::IsValid(NPC))
    {
        return Result = EBTNodeResult::Failed;
    }

    UNavigationSystemV1* NS = UNavigationSystemV1::GetNavigationSystem(NPC->GetWorld());
    if (false == ::IsValid(NS))
    {
        return Result = EBTNodeResult::Failed;
    }

    FVector StartPatrolPosition = OwnerComp.GetBlackboardComponent()->GetValueAsVector(ASAIController::StartPatrolPositionKey);
    FNavLocation EndPatrolLocation;
    if (true == NS->GetRandomReachablePointInRadius(FVector::ZeroVector, AIController->PatrolRadius, EndPatrolLocation))
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsVector(ASAIController::EndPatrolPositionKey, EndPatrolLocation.Location);
        return Result = EBTNodeResult::Succeeded;
    }

    return Result;
}