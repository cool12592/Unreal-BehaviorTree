// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Hited.h"
#include "MyAIController.h"
#include "BasicEnemy.h"


UBTTask_Hited::UBTTask_Hited()
{
    bNotifyTick = true;
   
}
EBTNodeResult::Type UBTTask_Hited::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

    controllingEnemy = Cast<ABasicEnemy>(OwnerComp.GetAIOwner()->GetPawn());
    if (controllingEnemy == nullptr)
        return EBTNodeResult::Failed;

    return EBTNodeResult::InProgress;
}

void UBTTask_Hited::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
  
    if (controllingEnemy == nullptr)
        return;

    if (controllingEnemy->isHited == false)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
    }
 
}
