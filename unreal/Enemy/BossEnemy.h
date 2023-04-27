// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasicEnemy.h"
#include "BossEnemy.generated.h"

/**
 * 
 */
UCLASS()
class NOTEBOOK_API ABossEnemy : public ABasicEnemy
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
private:
	void RunCoolTime(float DeltaTime);
	void CalculateDistFromPlayer();
	void CheckSuddenAttack();
	void CheckBackAttack();
	void Turn();
	bool CheckNearGround();
	void BossDie();
public:

	ABossEnemy();
	virtual void Attack() override;
	UFUNCTION(BlueprintCallable)
	void Boss_AttackCheck();

	UFUNCTION(NetMulticast, Reliable)
		 void Attack1_Multicast();
	UFUNCTION(NetMulticast, Reliable)
		 void Attack2_Multicast();
	UFUNCTION(NetMulticast, Reliable)
		 void Attack3_Multicast();
	UFUNCTION(NetMulticast, Reliable)
		 void Attack4_Multicast();
	UFUNCTION(NetMulticast, Reliable)
		 void Attack5_Multicast();

	UPROPERTY(EditDefaultsOnly, Category = Pawn)
		UAnimMontage* attackAnim1;
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
		UAnimMontage* attackAnim2;
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
		UAnimMontage* attackAnim3;
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
		UAnimMontage* attackAnim4;
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
		UAnimMontage* attackAnim5;
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
		UAnimMontage* LDodgeAnim;
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
		UAnimMontage* RDodgeAnim;
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
		UAnimMontage* DieAnim;


	UPROPERTY(EditDefaultsOnly, Category = Pawn)
		UAnimMontage* LTurnAnim;
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
		UAnimMontage* RTurnAnim;
	
	
	UFUNCTION(BlueprintCallable)
		void RotationArroundToPlayer();
	UFUNCTION(BlueprintCallable)
		void LaunchToPlayer();
	UFUNCTION()
		void ProjectToTarget();
	UFUNCTION()
		void TickParabola(float delta);

	virtual void MyTakeDamage(AActor* attacker, float damage, EnemyHitedState hit, float hitedTime_ = 0.f, FVector launchVec = FVector(0.f, 0.f, 0.f), FName note = TEXT("")) override;


	bool isParabola=false;
	bool isCheckNearGround=false;
	
	float gravitational_acceleration=8.f;
	float accumulate_down_vertical_force=0.f;
	float vertical_force=140.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isRotationArroundToPlayer = false;

	float TurnCoolTime = 2.f;
	float backAttackCoolTime = 2.f;
	float suddenAttackCoolTime = 2.f;

	FVector startLocation;
	FVector targetLocation;
	float distance;

};
