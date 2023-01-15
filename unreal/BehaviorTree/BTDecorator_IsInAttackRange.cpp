// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_IsInAttackRange.h"
#include "MyAIController.h"
#include "PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BasicEnemy.h"

UBTDecorator_IsInAttackRange::UBTDecorator_IsInAttackRange()
{
    NodeName = TEXT("CanAttack");
}

bool UBTDecorator_IsInAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

    auto Enemy = Cast<ABasicEnemy>(OwnerComp.GetAIOwner()->GetPawn());
    if (nullptr == Enemy)
        return false;

    auto Target = Cast<APlayerCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AMyAIController::TargetKey));
    if (nullptr == Target)
        return false;

    bResult = (Target->GetDistanceTo(Enemy) <= Enemy->attackRange);
    Enemy->SetPlayerDistance(Target->GetDistanceTo(Enemy));
    
   if ( Enemy->cooltime > 0) return false;


    return bResult;
}
