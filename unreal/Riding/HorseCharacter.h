// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HorseCharacter.generated.h"

UCLASS()
class NOTEBOOK_API AHorseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AHorseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UFUNCTION()
		void MoveForWard(float value);
	UFUNCTION()
		void MoveRight(float value);
	UFUNCTION()
	void StartJump();

	UPROPERTY()
	FVector oldDirection;
	UPROPERTY()
		FVector oldDirection2;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int isRight = 0;
			
	UPROPERTY()
	FVector oldForward;


	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	bool enable_move = true;


	UPROPERTY()
	float direction_count = 0.f;
};
