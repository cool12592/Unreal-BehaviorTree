// Fill out your copyright notice in the Description page of Project Settings.


#include "NormalSkill.h"
#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "MyMatineeCameraShake.h"
#include "Engine.h"
#include "MyAnimInstance.h"
#include "BasicEnemy.h"
#include "PlayerStatus.h"

#define LastAirComboCount 6
#define LastWaveCount 5
#define LaunchPower 1500

// Sets default values for this component's properties
UNormalSkill::UNormalSkill()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	airComboPosArray.Init(FVector(0.f, 0.f, 0.f), 4);
	airComboRotArray.Init(FRotator(0.f, 0.f, 0.f), 4);
}



// Called when the game starts
void UNormalSkill::BeginPlay()
{
	Super::BeginPlay();
	SetAllState(true);
	status = myplayer->FindComponentByClass<UPlayerStatus>();
}

bool UNormalSkill::CheckNormalSkillCondition(SkillName skillName)
{
	if (myplayer->enable_attack == false) return false;
	if (myplayer->GetCharacterMovement()->IsMovingOnGround() == false) return false;
	if (myplayer->CheckCoolTimeAndStamina(skillName) == false) return false;
	return true;
}

void UNormalSkill::ConsumeNormalSkillCondition(SkillName skillName)
{
	myplayer->SetAllState(false);
	myplayer->ConsumeCoolTimeAndStamina(skillName);
}

void UNormalSkill::SetAllState(bool b)
{
	myplayer->SetAllState(b);
}

void UNormalSkill::CountReset()
{
	airLaunchCount = 0;
	airComboCount = 0;
	myplayer->GetCharacterMovement()->GravityScale = 1.f;
	myplayer->Offinvincibility();
}

void UNormalSkill::Skill_DashAttack_Implementation()
{
	Skill_DashAttack_Multicast();
}

void UNormalSkill::Skill_DashAttack_Multicast_Implementation()
{
	if (CheckNormalSkillCondition(SkillName::dash) == false) return;
	ConsumeNormalSkillCondition(SkillName::dash);

	myplayer->PlayAnimMontage(myplayer->dashAttackAnim, 1.0f);
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = myplayer;
	FRotator rotator(0.f, 0.f, 0.f);
	FVector  SpawnLocation = myplayer->GetActorLocation() + myplayer->GetActorForwardVector() * 100.f;
	GetWorld()->SpawnActor<AActor>(myplayer->dashAttachBP, SpawnLocation + FVector(0.0f, 0.f, 0.f), myplayer->GetActorRotation(), SpawnParams);

}

void UNormalSkill::Skill_AirLaunch_Implementation()
{
	Skill_AirLaunch_Multicast();
}

void UNormalSkill::Skill_AirLaunch_Multicast_Implementation()
{
	if (CheckNormalSkillCondition(SkillName::airLaunch) == false) return;
	ConsumeNormalSkillCondition(SkillName::airLaunch);

	myplayer->SetActorEnableCollision(false);
	myplayer->GetCharacterMovement()->StopMovementImmediately();
	myplayer->LaunchCharacter(FVector(0, 0, LaunchPower), false, false);

	myplayer->PlayAnimMontage(myplayer->airLaunchAnim, 1.0f);

	FTimerHandle tempWaitHandle;
	float WaitTime = 1.f; 
	GetWorld()->GetTimerManager().SetTimer(tempWaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			myplayer->SetActorEnableCollision(true);
		}), WaitTime, false);
}

void UNormalSkill::Skill_AirLaunch_Check()
{
	airLaunchCount++;

	TArray<FHitResult> HitResults;
	float radius = 250.f;
	FVector start = myplayer->GetActorLocation();
	FVector end = myplayer->GetActorLocation() + myplayer->GetActorForwardVector() * radius;
	bool bResult = SweepTrace(start, end, radius, HitResults);
	
	if (bResult)
	{
		for (const FHitResult& HitResult : HitResults)
		{
			ABasicEnemy* enemyActor = Cast<ABasicEnemy>(HitResult.Actor);
			if (enemyActor)
			{
				if (airLaunchCount == 1)
				{
					enemyActor->GetCharacterMovement()->StopMovementImmediately();
					enemyActor->MyTakeDamage(myplayer, status->GetSkillDamage(SkillName::airLaunch), EnemyHitedState::RightLeft, 3.f, FVector(0.f, 0.f, 1420.f));
				}
				else
					enemyActor->MyTakeDamage(myplayer, status->GetSkillDamage(SkillName::airLaunch), EnemyHitedState::RightLeft, 3.f);
			}
		}
	}

	MyCameraShake(10.f);
}


void UNormalSkill::AirCombo_Implementation()
{
	AirCombo_Multicast();
}

void UNormalSkill::AirCombo_Multicast_Implementation()
{
	if (airComboCount == 0)
	{
		if (myplayer->CheckCoolTimeAndStamina(SkillName::airCombo) == false) return;
		ConsumeNormalSkillCondition(SkillName::airCombo);

		if (myplayer->GetCharacterMovement()->IsMovingOnGround() == false) //이미공중이면 첫번째동작은 노필요
		{
			airComboCount++; //첫번째동작거름
			FixedPositionInTheAir(myplayer);
		}

		airComboCount++;
		myplayer->PlayAnimMontage(myplayer->airComboAnim, 1.f, FName(*FString::FromInt(airComboCount)));

		airComboMeleeInputOn = false;
	}
	else //공격중
	{
		airComboMeleeInputOn = true;
	}

}

void UNormalSkill::AirComboNextCombo()
{
	if (airComboMeleeInputOn)
	{
		airComboMeleeInputOn = false;

		int arrayInd = (airComboCount - 1) % 4;
		myplayer->GetMesh()->SetRelativeLocationAndRotation(airComboPosArray[arrayInd], airComboRotArray[arrayInd]);

		airComboCount++;
		myplayer->PlayAnimMontage(myplayer->airComboAnim, 1.f, FName(*FString::FromInt(airComboCount)));

		if (airComboCount == 2)
			FixedPositionInTheAir(myplayer);

		MyCameraShake(7.f);
	}
	else
	{
		myplayer->GetMesh()->SetRelativeLocationAndRotation(airComboPosArray[0], airComboRotArray[0]);
	}
}

void UNormalSkill::AirComboCheck()
{
	if (airComboCount == 1)
		myplayer->LaunchCharacter(FVector(0, 0, LaunchPower), false, false);

	TArray<FHitResult> HitResults;
	float range = 100.f;
	float radius = 200.f;
	FVector start = myplayer->GetActorLocation();
	FVector end = myplayer->GetActorLocation() + myplayer->GetActorForwardVector() * range;
	bool bResult = SweepTrace(start, end, radius, HitResults);

	lastHitEnemy = nullptr;

	if (bResult)
	{
		for (const FHitResult& HitResult : HitResults)
		{
			ABasicEnemy* enemyActor = Cast<ABasicEnemy>(HitResult.Actor);
			if (enemyActor)
			{
				lastHitEnemy = enemyActor;
				if (enemyActor->nowHitedState == EnemyHitedState::knock) continue;

				if (airComboCount != LastAirComboCount)
				{
					enemyActor->MyTakeDamage(myplayer, status->GetSkillDamage(SkillName::airCombo), EnemyHitedState::RightLeft, 2.f);
					enemyActor->LandingTimer(); //내가 때리다 말수있으므로 착륙 타이머시킴

					if (airComboCount == 1)
						enemyActor->LaunchCharacter(FVector(0, 0, LaunchPower), false, false);
					else if (airComboCount == 2)
						FixedPositionInTheAir(enemyActor);
				}
				else
				{
					enemyActor->GetCharacterMovement()->GravityScale = 1.f;
					enemyActor->MyTakeDamage(myplayer, status->GetSkillDamage(SkillName::airCombo), EnemyHitedState::knock, 3.f, FVector(0, 0, -300.f), TEXT("SWORD_DANCE_READY"));
					enemyActor->LandingTimer_Off();//타이머중인건꺼야지혼선방지
				}
			}
		}
	}

	CheckFinishAttack();
	MyCameraShake(7.f);
}

void UNormalSkill::CheckFinishAttack()
{
	if (airComboCount == LastAirComboCount)
	{
		if (lastHitEnemy != nullptr)
		{
			SwordDance();
		}
		else
		{
			CountReset();
			SetAllState(true);
		}

	}
}

void UNormalSkill::MyCameraShake(float power)
{
	if (myplayer->GetController() == UGameplayStatics::GetPlayerController(GetWorld(), 0))
		GetWorld()->GetFirstPlayerController()->ClientPlayCameraShake(UMyMatineeCameraShake::StaticClass(), power, ECameraAnimPlaySpace::CameraLocal);
}

bool UNormalSkill::SweepTrace(FVector start, FVector end, float radius, TArray<FHitResult>& HitResults)
{
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(myplayer);

	bool bResult = GetWorld()->SweepMultiByChannel(
		HitResults,
		start,
		end,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(radius),
		Params);

	return bResult;
}

void UNormalSkill::SwordDance()
{
	FOutputDeviceNull pAR3;
	myplayer->CallFunctionByNameWithArguments(TEXT("TimeDilationEffect"), pAR3, nullptr, true);

	myplayer->myAnim->Montage_SetPlayRate(myplayer->GetCurrentMontage(), 0.2f);
	auto* particle1 = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), myplayer->Particle_teleportCharge, myplayer->GetActorLocation() + FVector(0.f, -70.f, 0.f));
	particle1->CustomTimeDilation = 3.f;

	float WaitTime = 0.7f;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			myplayer->CustomTimeDilation = 0.1f;
			Skill_Enemy_Slowed();

			float WaitTime2 = 1.6f;

			GetWorld()->GetTimerManager().SetTimer(WaitHandle2, FTimerDelegate::CreateLambda([&]()
				{
					Skill_Enemy_fixed();
					auto* particle2 = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), myplayer->Particle_teleportEnter, myplayer->GetActorLocation());
					particle2->CustomTimeDilation = 0.5f;
					myplayer->SetActorLocation(lastHitEnemy->GetActorLocation() - myplayer->GetActorForwardVector() * 300.f);
					FixedPositionInTheAir(myplayer);
					auto* particle3 = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), myplayer->Particle_teleportExit, myplayer->GetActorLocation());
					particle3->CustomTimeDilation = 0.5f;

					Skill_SwordDance();
					myplayer->CustomTimeDilation = 1.f;

				}), WaitTime2, false);

		}), WaitTime, false);
}

void UNormalSkill::Skill_SwordWave_Multicast_Implementation()
{

	if (CheckNormalSkillCondition(SkillName::swordWave))
		return;

	ConsumeNormalSkillCondition(SkillName::swordWave);
	myplayer->Oninvincibility();
	FOutputDeviceNull pAR;
	myplayer->CallFunctionByNameWithArguments(TEXT("Camera_enlargement"), pAR, nullptr, true);

	swordWaveRepeatCount = 0;
	waveCount = 0;
	myplayer->GetWorldTimerManager().ClearTimer(waveWaitHandle);
	myplayer->PlayAnimMontage(myplayer->swordWaveAnim, 1.0f);

}



void UNormalSkill::Skill_SwordWavePause()
{
	if (swordWaveRepeatCount != 0)
		return;

	myplayer->myAnim->Montage_Pause(myplayer->GetCurrentMontage());


	UParticleSystem* particle;
	float delay;
	if (myplayer->isDashComboCharge)
	{
		particle = myplayer->Particle_teleportCharge;
		delay = 2.f;
	}
	else
	{
		particle = myplayer->Particle_teleportCharge2;
		delay = 3.5f;
	}
	auto* particleActor = UGameplayStatics::SpawnEmitterAttached(particle, myplayer->GetMesh(), FName("Pelvis"), FVector(0.f, -130.f, 50.f), FRotator::ZeroRotator, FVector(2.F));
	particleActor->CustomTimeDilation = delay;

	float WaitTime = 2.f;

	if (myplayer->isDashComboCharge == true)
		WaitTime = 10.f;

	GetWorld()->GetTimerManager().SetTimer(waveWaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			myplayer->myAnim->Montage_Resume(myplayer->GetCurrentMontage());

		}), WaitTime, false);

}

void UNormalSkill::Skill_SwordWaveRepeat()
{
	waveCount++;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = myplayer;
	FRotator rotator = myplayer->GetActorRotation();
	FVector  SpawnLocation = myplayer->GetActorLocation();// -myplayer->GetActorUpVector() * 150.f;
	//SpawnLocation.Z -= 90.0f;
	auto* wave = GetWorld()->SpawnActor<AActor>(myplayer->waveActorBP, SpawnLocation, rotator, SpawnParams);

	FString waveName;
	FOutputDeviceNull OutputDeviceNull;
	if (waveCount == LastWaveCount)
		waveName = "BigWaveSpawn";
	else
	{
		waveName = "Wave";
		waveName.Append(FString::FromInt(waveCount));
		waveName.Append("_Spawn");
	}
	const TCHAR* CmdAndParams = *waveName;
	wave->CallFunctionByNameWithArguments(CmdAndParams, OutputDeviceNull, nullptr, true);
}


void UNormalSkill::Skill_SwordDance()
{
	myplayer->PlayAnimMontage(myplayer->swordDanceAnim, 1.0f);
}

void UNormalSkill::Skill_SwordDanceCheck()
{
	TArray<FHitResult> HitResults;
	float range = 400.f;
	float radius = 400.f;
	FVector start = myplayer->GetActorLocation();
	FVector end = myplayer->GetActorLocation() + myplayer->GetActorForwardVector() * range;
	bool bResult = SweepTrace(start, end, radius, HitResults);

	if (bResult)
	{
		for (const FHitResult& HitResult : HitResults)
		{
			ABasicEnemy* enemyActor = Cast<ABasicEnemy>(HitResult.Actor);
			if (enemyActor && !enemyActor->GetCharacterMovement()->IsMovingOnGround())
			{
				enemyActor->CustomTimeDilation = 1.f;
				enemyActor->MyTakeDamage(myplayer, status->GetSkillDamage(SkillName::airCombo), EnemyHitedState::knock, 1.f);
			}
		}
	}
	MyCameraShake(20.f);
}

void UNormalSkill::Skill_Enemy_fixed()
{

	TArray<FHitResult> HitResults;
	float range = 800.f;
	float radius = 800.f;
	FVector start = myplayer->GetActorLocation();
	FVector end = myplayer->GetActorLocation() + myplayer->GetActorForwardVector() * range;
	bool bResult = SweepTrace(start, end, radius, HitResults);

	if (bResult)
	{
		for (const FHitResult& HitResult : HitResults)
		{
			ABasicEnemy* enemyActor = Cast<ABasicEnemy>(HitResult.Actor);
			if (enemyActor && !enemyActor->GetCharacterMovement()->IsMovingOnGround())
			{
				FixedPositionInTheAir(enemyActor);
				enemyActor->LandingTimer(2.5f);
			}
		}
	}
}


void UNormalSkill::Skill_Enemy_Slowed()
{
	TArray<FHitResult> HitResults;
	float range = 500.f;
	float radius = 500.f;
	FVector start = myplayer->GetActorLocation();
	FVector end = myplayer->GetActorLocation() + myplayer->GetActorForwardVector() * range;
	bool bResult = SweepTrace(start, end, radius, HitResults);

	if (bResult)
	{
		for (const FHitResult& HitResult : HitResults)
		{
			ABasicEnemy* enemyActor = Cast<ABasicEnemy>(HitResult.Actor);
			if (enemyActor && !enemyActor->GetCharacterMovement()->IsMovingOnGround())
			{
				enemyActor->CustomTimeDilation = 0.07f;
				enemyActor->LandingTimer(3.f);

			}
		}
	}
}

void UNormalSkill::FixedPositionInTheAir(ACharacter* character)
{
	character->GetCharacterMovement()->GravityScale = 0.f;
	character->GetCharacterMovement()->StopMovementImmediately();
}




void UNormalSkill::Skill_DashCombo_Multicast_Implementation()
{
	myplayer->isDashComboCharge = true;
	//대쉬콤보스킬이라면
	if ((myplayer->GetCharacterMovement()->IsMovingOnGround() && myplayer->DashCombo_StateCheck() &&
		myplayer->CheckCoolTimeAndStamina(SkillName::dashCombo)) == false)
	{
		myplayer->isDashComboCharge = false; //실패했단것
		return;
	}
	myplayer->ConsumeCoolTimeAndStamina(SkillName::dashCombo);
	myplayer->Oninvincibility();
	FOutputDeviceNull pAR;
	myplayer->CallFunctionByNameWithArguments(TEXT("Camera_enlargement"), pAR, nullptr, true);


	FOutputDeviceNull pAR2;
	myplayer->CallFunctionByNameWithArguments(TEXT("SkillDashComboInBP"), pAR2, nullptr, true);

	swordWaveRepeatCount = 0;
	waveCount = 0;
	myplayer->GetWorldTimerManager().ClearTimer(waveWaitHandle);
	myplayer->PlayAnimMontage(myplayer->swordWaveAnim, 1.0f);
}