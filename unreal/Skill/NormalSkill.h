// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NormalSkill.generated.h"

enum class SkillName : uint8;

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

	//condition
	bool CheckNormalSkillCondition(SkillName skillName);
	void ConsumeNormalSkillCondition(SkillName skillName);

	UFUNCTION(BlueprintCallable)
	void SetAllState(bool b);
	UFUNCTION(BlueprintCallable)
	void CountReset();

	////////
	//skill/
	////////

	//DashAttack
	UFUNCTION(Server, Reliable)
		void Skill_DashAttack();
	UFUNCTION(NetMulticast, Reliable)
		void Skill_DashAttack_Multicast();

	//AirLaunch
	UFUNCTION(Server, Reliable)
		void Skill_AirLaunch();
	UFUNCTION(NetMulticast, Reliable)
		void Skill_AirLaunch_Multicast();
	UFUNCTION()
		void Skill_AirLaunch_Check();

	//AirCombo
	UFUNCTION(Server, Reliable)
		void AirCombo();
	UFUNCTION(NetMulticast, Reliable)
		void AirCombo_Multicast();
	UFUNCTION()
		void AirComboNextCombo();
	UFUNCTION()
		void AirComboCheck();
	UFUNCTION()
		void Skill_SwordDanceCheck();

	//SwordWave
	UFUNCTION(NetMulticast, Reliable)
		void Skill_SwordWave_Multicast();
	UFUNCTION()
		void Skill_SwordWavePause();
	UFUNCTION()
		void Skill_SwordWaveRepeat();
	UFUNCTION(NetMulticast, Reliable)
		void Skill_DashCombo_Multicast();

private:
	void FixedPositionInTheAir(ACharacter* character);
	void SwordDance();

	void Skill_SwordDance();
	void Skill_Enemy_fixed();
	void Skill_Enemy_Slowed();
	void CheckFinishAttack();
	void MyCameraShake(float power);
	bool SweepTrace(FVector start, FVector end, float radius, TArray<FHitResult>& HitResults);

private:
	int airComboCount;
	bool airComboMeleeInputOn;
	int swordWaveRepeatCount;
	int waveCount;
	float chargeAnimPauseTime;
	int airLaunchCount;

	UPROPERTY()
		class ABasicEnemy* lastHitEnemy;
	UPROPERTY()
		class UPlayerStatus* status;

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
