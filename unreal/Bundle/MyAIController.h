// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MyAIController.generated.h"

/**
 * 
 */
UCLASS()
class NOTEBOOK_API AMyAIController : public AAIController
{
	GENERATED_BODY()
public:
    AMyAIController();
    virtual void OnPossess(APawn* InPawn) override;
 
    static const FName HomePosKey;
    static const FName PatrolPosKey;
    static const FName TargetKey;

    UFUNCTION()
        void SetTarget(AActor* actor);
private:
    UPROPERTY()
        class UBehaviorTree* BTAsset;

    UPROPERTY()
        class UBlackboardData* BBAsset;
	
};
