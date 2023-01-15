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

   
    auto my = Cast<ABasicEnemy>(OwnerComp.GetAIOwner()->GetPawn());
    if (nullptr == my)
        return EBTNodeResult::Failed;
    auto Target = Cast<APlayerCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AMyAIController::TargetKey));
    if (nullptr == Target)
        return EBTNodeResult::Failed;
  

    FVector LookVector = Target->GetActorLocation() - my->GetActorLocation();
    LookVector.Z = 0.0f; //rotator말고 위치의z임
    TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();

    if (my->IsAttacking == false) //공격전에 도는 회전
    {
        /*	FLatentActionInfo LatentInfo;
	    LatentInfo.CallbackTarget = this;
	    UKismetSystemLibrary::MoveComponentTo(my->GetRootComponent(), my->GetActorLocation() , TargetRot, true, true, 0.1f, true, EMoveComponentAction::Move, LatentInfo);
        return EBTNodeResult::Succeeded;*/

        return EBTNodeResult::InProgress;

    }
    
    //공격중

    else
    {
        if (my->turn_during_attacking ) //공격중인데 turn_during_attacking가 아니면 리턴
        {
            //공격중 PARALLEL으로 회전
            my->SetActorRotation(FMath::RInterpTo(my->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), my->TurnSpeed));
        }

        return EBTNodeResult::Succeeded;
    }



 


    return EBTNodeResult::Succeeded;


  // 어차피 공격중에는 turn_during_attacking 아닌애는안하고 맞는애는 PARALLEL에서 비동기적으로 계속동작하니 TICK까지 갈필요없음 
 //생각해보니 TICK한이유가 공격전에 방향맞추려한건데 그것도 그냥 MOVETOCOMPOENTN로 공격전에 하면되겠는데
    
        
   

    

 
}

void UBTTAsk_TurnToTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);


    auto my = Cast<ABasicEnemy>(OwnerComp.GetAIOwner()->GetPawn());
    if (nullptr == my)
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

    auto Target = Cast<APlayerCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AMyAIController::TargetKey));
    if (nullptr == Target)
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

    FVector LookVector = Target->GetActorLocation() - my->GetActorLocation();
    LookVector.Z = 0.0f; //rotator말고 위치의z임
    TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
    my->SetActorRotation(FMath::RInterpTo(my->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 5.F));


    if (my->GetActorRotation().GetManhattanDistance(TargetRot) <=10.f)
    {

        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }


}
