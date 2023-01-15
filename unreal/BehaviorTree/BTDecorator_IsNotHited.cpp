// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_IsNotHited.h"
#include "MyAIController.h"
#include "BasicEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_IsNotHited::UBTDecorator_IsNotHited()
{
    NodeName = TEXT("NotHited");
}

bool UBTDecorator_IsNotHited::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

    auto controllingEnemy = Cast<ABasicEnemy>(OwnerComp.GetAIOwner()->GetPawn());
    if (controllingEnemy == nullptr)
        return false;
 
    bResult = !controllingEnemy->isHited;
    return bResult;
}

