// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"
#include "MyAIController.h"
#include "BasicEnemy.h"

UBTTask_Attack::UBTTask_Attack()
{
    bNotifyTick = true;
//    IsAttacking = false;
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

    auto Enemy = Cast<ABasicEnemy>(OwnerComp.GetAIOwner()->GetPawn());
    if (nullptr == Enemy)
        return EBTNodeResult::Failed;

    Enemy->Attack();
  /*  ABCharacter->OnAttackEnd.AddLambda([this]() -> void {
        IsAttacking = false;
        });*/

    return EBTNodeResult::InProgress;
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
    auto Enemy = Cast<ABasicEnemy>(OwnerComp.GetAIOwner()->GetPawn());
    if (nullptr == Enemy)
        return;
    if (!Enemy->IsAttacking || Enemy->isHited)
    {

        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
//    else
  //      GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("aaaaaaaaaaaaaaaa")); // 화면출력

}
