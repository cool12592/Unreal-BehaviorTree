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
    //    IsAttacking = false;
}

EBTNodeResult::Type UBTTask_MyMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

  
    UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
    if (NavSystem == nullptr)
        return EBTNodeResult::Failed;
    auto Enemy = Cast<ABasicEnemy>(OwnerComp.GetAIOwner()->GetPawn());

    auto* target = Cast<APlayerCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AMyAIController::TargetKey));
    if (target == nullptr)
    {
        UAIBlueprintHelperLibrary::SimpleMoveToLocation(Enemy->GetController(), OwnerComp.GetBlackboardComponent()->GetValueAsVector(AMyAIController::PatrolPosKey));
          
        start = true;
    }
   

    


   

    return EBTNodeResult::InProgress;
}

void UBTTask_MyMoveTo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
   
    auto Enemy = Cast<ABasicEnemy>(OwnerComp.GetAIOwner()->GetPawn());
  
    if (nullptr == Enemy)
        return;



    //3���� ��Ȳ  1. ������Ȥ�� ���ݹ޴� ���¸� �̵�x
    //            2. �÷��̾� Ÿ���� ������� A. ���� ���ɻ��¸� �̵��ߴ�. B ���ݺҰ��ɻ����ε� ���������� ���� C. ���ݺҰ����ε� ���� ���� �ƴϸ� �ߴ�
    //            3. �÷��̾� Ÿ�� ������ �׳� ��Ʈ�� �������� �̵�

    if (Enemy->IsAttacking || Enemy->isHited)
    {
        //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("111")); // ȭ�����

        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;

    }
    

    auto* player_target = Cast<APlayerCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AMyAIController::TargetKey));
    if (player_target)
    {
        
        //���ݰ��� ���¸� �ߴ�
        if ((Enemy->cooltime <= 0.f && player_target->GetDistanceTo(Enemy) <= Enemy->attackRange)) //���ݰ���
        {
          //  GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("2222222")); // ȭ�����

            FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

        }

        else   
        {
            if ((player_target->GetDistanceTo(Enemy) > Enemy->TrackToRange)) //�������� Ȯ�� (������ �����Ұų�)
            {
              //  GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("33333333")); // ȭ�����

                UAIBlueprintHelperLibrary::SimpleMoveToLocation(Enemy->GetController(), player_target->GetActorLocation());
            }
            else
            {
               // GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("4444444444")); // ȭ�����

                UAIBlueprintHelperLibrary::SimpleMoveToLocation(Enemy->GetController(), Enemy->GetActorLocation());//�ߴ�

            }

        }
     


    }


    //3. �÷��̾�Ÿ���� ������ �׳� ��Ʈ����ġ ���� �ӵ�0�ǰ� �ߴ�
    if (start && Enemy->GetVelocity().Size() < 1.f) //goal_pos== Enemy->GetActorLocation())
    {
      // GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("55555555")); // ȭ�����

        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

    }


}
