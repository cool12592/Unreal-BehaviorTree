// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTAsk_TurnToTarget.h"
#include "MyAIController.h"
#include "PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BasicEnemy.h"
#include "Kismet/KismetSystemLibrary.h"

UBTTAsk_TurnToTarget::UBTTAsk_TurnToTarget()
{
    NodeName = TEXT("Turn");
    bNotifyTick = true;
}

              
EBTNodeResult::Type UBTTAsk_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

   
    controllingEnemy = Cast<ABasicEnemy>(OwnerComp.GetAIOwner()->GetPawn());
    if (controllingEnemy == nullptr)
        return EBTNodeResult::Failed;

    auto playerTarget = Cast<APlayerCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AMyAIController::TargetKey));
    if (playerTarget == nullptr)
        return EBTNodeResult::Failed;
  
    if (controllingEnemy->IsAttacking == false) //공격전에 도는 회전
    {
        return EBTNodeResult::InProgress;
    }
    else //공격중
    {
        if (controllingEnemy->isTurnDuringAttacking) //공격중에 돌 수 있는 enemy
        {
            TurnToPlayerTarget(playerTarget);
        }
    }

    // 어차피 공격중에는 isTurnDuringAttacking 아닌애는안하고 맞는애는 PARALLEL에서 비동기적으로 계속동작하니 TICK까지 갈필요없음 
    return EBTNodeResult::Succeeded;

 //생각해보니 TICK한이유가 공격전에 방향맞추려한건데 그것도 그냥 MOVETOCOMPOENTN로 공격전에 하면되겠는데
}

void UBTTAsk_TurnToTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    if (controllingEnemy == nullptr)
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

    auto playerTarget = Cast<APlayerCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AMyAIController::TargetKey));
    if (playerTarget == nullptr)
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

    TurnToPlayerTarget(playerTarget);

    if (controllingEnemy->GetActorRotation().GetManhattanDistance(TargetRot) <= 10.f)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}

void UBTTAsk_TurnToTarget::TurnToPlayerTarget(APlayerCharacter* playerTarget)
{
    FVector LookVector = playerTarget->GetActorLocation() - controllingEnemy->GetActorLocation();
    LookVector.Z = 0.0f; //rotator말고 위치의z임
    TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
    controllingEnemy->SetActorRotation(FMath::RInterpTo(controllingEnemy->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 5.F));
}
