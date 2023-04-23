// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NormalSkill.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NOTEBOOK_API UNormalSkill : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UNormalSkill();
	class APlayerCharacter* myplayer;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:
	/** Allow each actor to run at a different time speed. The DeltaTime for a frame is multiplied by the global TimeDilation (in WorldSettings) and this CustomTimeDilation for this actor's tick.  */
//	UPROPERTY(BlueprintReadWrite, AdvancedDisplay, Category = Actor)
	//	float CustomTimeDilation;

	UFUNCTION(BlueprintCallable)
		void SetAllState(bool b);
	UFUNCTION(BlueprintCallable)
		void CountReset();
	//skill
	
	UFUNCTION(NetMulticast, Reliable)
		void AirCombo_Multicast();
	UFUNCTION()
		void AirComboNextCombo();
	UFUNCTION()
		void AirComboCheck();
	
	
	UFUNCTION(NetMulticast, Reliable)
		void Skill_SwordWave_Multicast();

	UFUNCTION(NetMulticast, Reliable)
		void Skill_DashCombo_Multicast();

	UFUNCTION()
		void Skill_SwordWavePause();
	UFUNCTION()
		void Skill_SwordWaveRepeat();

	void Skill_SwordDance();
	void Skill_SwordDanceCheck();
	void Skill_Enemy_fixed();
	void Skill_Enemy_Slowed();


private:
	int airComboCount = 0;
	bool airComboMeleeInputOn = false;
	int attackMeleeCount = 0;
	bool attackMeleeInputOn = false;
	class ABasicEnemy* lastHitEnemy = nullptr;
	int swordWaveRepeatCount = 0;
	int waveCount = 0;
	float chargeAnimPauseTime=0.f;

	FTimerHandle waveWaitHandle;
	FTimerHandle WaitHandle;
	FTimerHandle WaitHandle2;
	FTimerHandle WaitHandle_slow;

	TArray<FVector> airComboPosArray;
	TArray<FRotator> airComboRotArray;
};
