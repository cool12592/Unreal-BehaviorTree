// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

const FName AMyAIController::HomePosKey(TEXT("HomePos"));
const FName AMyAIController::PatrolPosKey(TEXT("PatrolPos"));
const FName AMyAIController::TargetKey(TEXT("Target"));

AMyAIController::AMyAIController()
{
    static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(TEXT("/Game/My__/Enemy/AI/BB_Enemy.BB_Enemy"));
    if (BBObject.Succeeded())
    {
        BBAsset = BBObject.Object;
    }

    static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("/Game/My__/Enemy/AI/BT_Enemy.BT_Enemy"));
    if (BTObject.Succeeded())
    {
        BTAsset = BTObject.Object;
    }
}

void AMyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    if (UseBlackboard(BBAsset, Blackboard))
    {
        Blackboard->SetValueAsVector(HomePosKey, InPawn->GetActorLocation());
        if (!RunBehaviorTree(BTAsset))
        {
            ensure(TEXT("AIController couldn't run behavior tree!"));
        }
    }
}
void AMyAIController::SetTarget(AActor* actor)
{
    //if (UseBlackboard(BBAsset, Blackboard)) 
    //    Blackboard->SetValueAsObject(TargetKey, actor);
}