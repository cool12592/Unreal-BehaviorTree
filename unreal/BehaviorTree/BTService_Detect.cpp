// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Detect.h"
#include "MyAIController.h"
#include "BasicEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "PlayerCharacter.h"
#include "BossEnemy.h"
#include "Kismet/GameplayStatics.h"
UBTService_Detect::UBTService_Detect()
{
    NodeName = TEXT("Detect");
    Interval = 1.0f;
}

void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    auto* my = Cast<ABasicEnemy>(OwnerComp.GetAIOwner()->GetPawn());
    if (nullptr == my)
        return;

    UWorld* World = my->GetWorld();
    FVector Center = my->GetActorLocation();
    float DetectRadius = my->detectRange;

    if (nullptr == World)
        return;

    TArray<FOverlapResult> OverlapResults;
    FCollisionQueryParams CollisionQueryParam(NAME_None, false, my);
    bool bResult = World->OverlapMultiByChannel(OverlapResults, Center, FQuat::Identity, ECollisionChannel::ECC_GameTraceChannel2, FCollisionShape::MakeSphere(DetectRadius), CollisionQueryParam);

   

    auto* target = Cast<APlayerCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AMyAIController::TargetKey));


 
    if (my->isBoss)
    {

        OwnerComp.GetBlackboardComponent()->SetValueAsObject(AMyAIController::TargetKey, UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
        return;
    }



    if (my->myTarget)
    {
       

     //   my->SetActorLocation( target->GetActorLocation()+ my->GetActorForwardVector()*150.f);
        OwnerComp.GetBlackboardComponent()->SetValueAsObject(AMyAIController::TargetKey, my->myTarget);

        return;

    }
    else if (bResult)
    {

        for (auto const& OverlapResult : OverlapResults)
        {
            APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(OverlapResult.GetActor());
            if (playerCharacter && playerCharacter->GetController()->IsPlayerController())
            {

                OwnerComp.GetBlackboardComponent()->SetValueAsObject(AMyAIController::TargetKey, playerCharacter);
             //   DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Green, false, 0.2f);

              //  DrawDebugPoint(World, playerCharacter->GetActorLocation(), 10.0f, FColor::Blue, false, 0.2f);
                //나(적)->상대방(플레이어) 까지 라인
              //  DrawDebugLine(World, my->GetActorLocation(), playerCharacter->GetActorLocation(), FColor::Blue, false, 0.2f);
            
                return;
            }
        }
    }
    else
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsObject(AMyAIController::TargetKey, NULL);
    }
      //  DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, 0.2f);
    
}
