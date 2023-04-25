// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NormalSkill.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
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

	UFUNCTION()
		void Skill_SwordDanceCheck();
private:
	void FixedPositionInTheAir(ACharacter* character);
	void SwordDance();

	void Skill_SwordDance();
	void Skill_Enemy_fixed();
	void Skill_Enemy_Slowed();
	void CheckFinishAttack();

private:
	int airComboCount;
	bool airComboMeleeInputOn;
	int attackMeleeCount;
	bool attackMeleeInputOn;
	int swordWaveRepeatCount;
	int waveCount;
	float chargeAnimPauseTime;

	UPROPERTY()
		class ABasicEnemy* lastHitEnemy;

	FTimerHandle waveWaitHandle;
	FTimerHandle WaitHandle;
	FTimerHandle WaitHandle2;
	FTimerHandle WaitHandle_slow;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AirCombo")
		TArray<FVector> airComboPosArray;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AirCombo")
		TArray<FRotator> airComboRotArray;
};
