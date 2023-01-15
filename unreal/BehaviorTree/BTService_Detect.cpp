// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Detect.h"
#include "MyAIController.h"
#include "BasicEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "PlayerCharacter.h"
#include "BossEnemy.h"
#include "Kismet/GameplayStatics.h"

#define DrawDebug 0
UBTService_Detect::UBTService_Detect()
{
    NodeName = TEXT("Detect");
    Interval = 1.0f;
}

void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    auto* controllingEnemy = Cast<ABasicEnemy>(OwnerComp.GetAIOwner()->GetPawn());
    if (controllingEnemy == nullptr)
        return;

    if (controllingEnemy->isBoss)
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsObject(AMyAIController::TargetKey, UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
        return;
    }

    UWorld* World = controllingEnemy->GetWorld();
    FVector Center = controllingEnemy->GetActorLocation();
    float DetectRadius = controllingEnemy->detectRange;

    if (World == nullptr)
        return;

    TArray<FOverlapResult> OverlapResults;
    FCollisionQueryParams CollisionQueryParam(NAME_None, false, controllingEnemy);
    bool bResult = World->OverlapMultiByChannel(OverlapResults, Center, FQuat::Identity, ECollisionChannel::ECC_GameTraceChannel2, FCollisionShape::MakeSphere(DetectRadius), CollisionQueryParam);


    if (controllingEnemy->myTarget)
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsObject(AMyAIController::TargetKey, controllingEnemy->myTarget);
    }
    else if (bResult)
    {
        for (auto const& OverlapResult : OverlapResults)
        {
            APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(OverlapResult.GetActor());
            if (playerCharacter && playerCharacter->GetController()->IsPlayerController())
            {
                OwnerComp.GetBlackboardComponent()->SetValueAsObject(AMyAIController::TargetKey, playerCharacter);
#if DrawDebug
                DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Green, false, 0.2f);
                DrawDebugPoint(World, playerCharacter->GetActorLocation(), 10.0f, FColor::Blue, false, 0.2f);
                //��(��)->����(�÷��̾�) ���� ����
                DrawDebugLine(World, my->GetActorLocation(), playerCharacter->GetActorLocation(), FColor::Blue, false, 0.2f);
#endif
            }
        }
    }
    else
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsObject(AMyAIController::TargetKey, NULL);
    }
}