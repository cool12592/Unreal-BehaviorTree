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
    LookVector.Z = 0.0f; //rotator���� ��ġ��z��
    TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();

    if (my->IsAttacking == false) //�������� ���� ȸ��
    {
        /*	FLatentActionInfo LatentInfo;
	    LatentInfo.CallbackTarget = this;
	    UKismetSystemLibrary::MoveComponentTo(my->GetRootComponent(), my->GetActorLocation() , TargetRot, true, true, 0.1f, true, EMoveComponentAction::Move, LatentInfo);
        return EBTNodeResult::Succeeded;*/

        return EBTNodeResult::InProgress;

    }
    
    //������

    else
    {
        if (my->turn_during_attacking ) //�������ε� turn_during_attacking�� �ƴϸ� ����
        {
            //������ PARALLEL���� ȸ��
            my->SetActorRotation(FMath::RInterpTo(my->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), my->TurnSpeed));
        }

        return EBTNodeResult::Succeeded;
    }



 


    return EBTNodeResult::Succeeded;


  // ������ �����߿��� turn_during_attacking �ƴѾִ¾��ϰ� �´¾ִ� PARALLEL���� �񵿱������� ��ӵ����ϴ� TICK���� ���ʿ���� 
 //�����غ��� TICK�������� �������� ������߷��Ѱǵ� �װ͵� �׳� MOVETOCOMPOENTN�� �������� �ϸ�ǰڴµ�
    
        
   

    

 
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
    LookVector.Z = 0.0f; //rotator���� ��ġ��z��
    TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
    my->SetActorRotation(FMath::RInterpTo(my->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 5.F));


    if (my->GetActorRotation().GetManhattanDistance(TargetRot) <=10.f)
    {

        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }


}
