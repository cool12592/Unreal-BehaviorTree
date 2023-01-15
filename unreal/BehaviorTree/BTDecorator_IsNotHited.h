// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_IsNotHited.generated.h"

/**
 * 
 */
UCLASS()
class NOTEBOOK_API UBTDecorator_IsNotHited : public UBTDecorator
{
	GENERATED_BODY()
public:
	UBTDecorator_IsNotHited();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
