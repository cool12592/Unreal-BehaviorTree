// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MyMoveTo.h"
#include "MyAIController.h"
#include "BasicEnemy.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_MyMoveTo::UBTTask_MyMoveTo()
{
    bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_MyMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

    controllingEnemy = Cast<ABasicEnemy>(OwnerComp.GetAIOwner()->GetPawn());
    if (controllingEnemy == nullptr)
        return EBTNodeResult::Failed;

    UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
    if (NavSystem == nullptr)
        return EBTNodeResult::Failed;

    auto* playerTarget = Cast<APlayerCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AMyAIController::TargetKey));
    if (playerTarget == nullptr)
    {
        UAIBlueprintHelperLibrary::SimpleMoveToLocation(controllingEnemy->GetController(), OwnerComp.GetBlackboardComponent()->GetValueAsVector(AMyAIController::PatrolPosKey));
        isFinishedPatrolSetting = true;
    }
 
    return EBTNodeResult::InProgress;
}

void UBTTask_MyMoveTo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
   
    if (controllingEnemy == nullptr)
        return;

    //3���� ��Ȳ  1. ������ Ȥ�� ���ݹ޴� ���¸� �̵�x
    //            2. �÷��̾� Ÿ���� ������� A. ���� ���ɻ��¸� �̵��ߴ�. B ���ݺҰ��ɻ����ε� ���������� ���� C. ���ݺҰ����ε� ���� ���� �ƴϸ� �ߴ�
    //            3. �÷��̾� Ÿ�� ������ �׳� ��Ʈ�� �������� �̵�

    bool isMovable = CheckMovableCondition();
    if (isMovable == false)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
    }
    else
    {
        auto* playerTarget = Cast<APlayerCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AMyAIController::TargetKey));
        if (playerTarget)
        {
            MoveToPlayerTarget(OwnerComp, playerTarget);
        }
        else  
        {
            MoveToPatrol(OwnerComp);
        }
    }
}

bool UBTTask_MyMoveTo::CheckMovableCondition()
{
    if (controllingEnemy->IsAttacking || controllingEnemy->isHited)
    {
        return false;
    }
    return true;
}

void UBTTask_MyMoveTo::MoveToPlayerTarget(UBehaviorTreeComponent& OwnerComp, APlayerCharacter* playerTarget)
{
    auto distance = playerTarget->GetDistanceTo(controllingEnemy);
    //���ݰ��� ���¸� �ߴ�
    if ((controllingEnemy->attackCoolTime <= 0.f && distance <= controllingEnemy->attackRange)) //���ݰ���
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
    }
    else
    {
        if (distance > controllingEnemy->TrackToRange) //�������� Ȯ�� (������ �����Ұų�)
        {
            UAIBlueprintHelperLibrary::SimpleMoveToLocation(controllingEnemy->GetController(), playerTarget->GetActorLocation()); //����
        }
        else
        {
            UAIBlueprintHelperLibrary::SimpleMoveToLocation(controllingEnemy->GetController(), controllingEnemy->GetActorLocation());//�̵��ߴ�(SimpleMoveToLocation���̻��ѹ����־�����ߴ���)
        }
    }
}



void UBTTask_MyMoveTo::MoveToPatrol(UBehaviorTreeComponent& OwnerComp)
{
    if (isFinishedPatrolSetting && controllingEnemy->GetVelocity().Size() < 1.f) //3��Ȳ �÷��̾�Ÿ���� ������ �׳� ��Ʈ����ġ ���� �ӵ�0�ǰ� �ߴ�
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}
