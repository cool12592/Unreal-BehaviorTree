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



    //3가지 상황  1. 공격중혹은 공격받는 상태면 이동x
    //            2. 플레이어 타겟이 있을경우 A. 공격 가능상태면 이동중단. B 공격불가능상태인데 추적범위면 추적 C. 공격불가능인데 추적 범위 아니면 중단
    //            3. 플레이어 타겟 없으면 그냥 패트롤 지점으로 이동

    if (Enemy->IsAttacking || Enemy->isHited)
    {
        //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("111")); // 화면출력

        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;

    }
    

    auto* player_target = Cast<APlayerCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AMyAIController::TargetKey));
    if (player_target)
    {
        
        //공격가능 상태면 중단
        if ((Enemy->cooltime <= 0.f && player_target->GetDistanceTo(Enemy) <= Enemy->attackRange)) //공격가능
        {
          //  GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("2222222")); // 화면출력

            FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

        }

        else   
        {
            if ((player_target->GetDistanceTo(Enemy) > Enemy->TrackToRange)) //추적범위 확인 (어디까지 추적할거냐)
            {
              //  GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("33333333")); // 화면출력

                UAIBlueprintHelperLibrary::SimpleMoveToLocation(Enemy->GetController(), player_target->GetActorLocation());
            }
            else
            {
               // GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("4444444444")); // 화면출력

                UAIBlueprintHelperLibrary::SimpleMoveToLocation(Enemy->GetController(), Enemy->GetActorLocation());//중단

            }

        }
     


    }


    //3. 플레이어타깃이 없으면 그냥 패트롤위치 가서 속도0되고 중단
    if (start && Enemy->GetVelocity().Size() < 1.f) //goal_pos== Enemy->GetActorLocation())
    {
      // GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("55555555")); // 화면출력

        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

    }


}
