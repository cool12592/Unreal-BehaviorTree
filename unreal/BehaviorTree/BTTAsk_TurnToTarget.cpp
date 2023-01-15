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
  
    if (controllingEnemy->IsAttacking == false) //�������� ���� ȸ��
    {
        return EBTNodeResult::InProgress;
    }
    else //������
    {
        if (controllingEnemy->isTurnDuringAttacking) //�����߿� �� �� �ִ� enemy
        {
            TurnToPlayerTarget(playerTarget);
        }
    }

    // ������ �����߿��� isTurnDuringAttacking �ƴѾִ¾��ϰ� �´¾ִ� PARALLEL���� �񵿱������� ��ӵ����ϴ� TICK���� ���ʿ���� 
    return EBTNodeResult::Succeeded;

 //�����غ��� TICK�������� �������� ������߷��Ѱǵ� �װ͵� �׳� MOVETOCOMPOENTN�� �������� �ϸ�ǰڴµ�
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
    LookVector.Z = 0.0f; //rotator���� ��ġ��z��
    TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
    controllingEnemy->SetActorRotation(FMath::RInterpTo(controllingEnemy->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 5.F));
}
