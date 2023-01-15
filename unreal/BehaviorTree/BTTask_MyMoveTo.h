// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MyMoveTo.generated.h"

/**
 * 
 */

UCLASS()
class NOTEBOOK_API UBTTask_MyMoveTo : public UBTTaskNode
{
	GENERATED_BODY()
public:
    UBTTask_MyMoveTo();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
   

protected:
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
      
private:
    bool CheckMovableCondition();
    void MoveToPlayerTarget(UBehaviorTreeComponent& OwnerComp, class APlayerCharacter* playerTarget);
    void MoveToPatrol(UBehaviorTreeComponent& OwnerComp);

    UPROPERTY()
    class ABasicEnemy* controllingEnemy;
    bool isFinishedPatrolSetting = false;
};
