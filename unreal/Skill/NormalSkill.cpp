// Fill out your copyright notice in the Description page of Project Settings.


#include "NormalSkill.h"
#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "MyMatineeCameraShake.h"
#include "Engine.h"
#include "MyAnimInstance.h"
#include "BasicEnemy.h"

#define LastAirComboCount 6
#define LastWaveCount 5

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
}

void UNormalSkill::SetAllState(bool b)
{
	myplayer->SetAllState(b);
	airComboCount = 0;
}

void UNormalSkill::CountReset()
{
	myplayer->CountReset();
}


void UNormalSkill::AirCombo_Multicast_Implementation()
{
	if (airComboCount == 0)
	{
		if (myplayer->CheckCoolTimeAndStamina(SkillName::airCombo) == false) return;

		if (myplayer->GetCharacterMovement()->IsMovingOnGround() == false) //이미공중이면 첫번째동작은 노필요
		{
			airComboCount++; //첫번째동작거름
			FixedPositionInTheAir(myplayer);
		}

		airComboCount++;
		myplayer->enable_attack = false;
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
		myplayer->GetMesh()->SetRelativeLocationAndRotation(airComboPosArray[arrayInd],airComboRotArray[arrayInd]);

		airComboCount++;
		myplayer->PlayAnimMontage(myplayer->airComboAnim, 1.f, FName(*FString::FromInt(airComboCount)));

		if (airComboCount == 2)
			FixedPositionInTheAir(myplayer);
	
		if (myplayer->GetController() == UGameplayStatics::GetPlayerController(GetWorld(), 0))
			GetWorld()->GetFirstPlayerController()->ClientPlayCameraShake(UMyMatineeCameraShake::StaticClass(), 7.f, ECameraAnimPlaySpace::CameraLocal);
	}
	else
	{
		myplayer->GetMesh()->SetRelativeLocationAndRotation(airComboPosArray[0], airComboRotArray[0]);
	}
}

void UNormalSkill::AirComboCheck()
{
	if (airComboCount == 1)
		myplayer->LaunchCharacter(FVector(0, 0, 1500), false, false);

	float attack_range = 100.f;
	float attact_radius = 200.f;
	TArray<FHitResult> HitResults;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(myplayer);

	bool bResult = GetWorld()->SweepMultiByChannel(
		HitResults,
		myplayer->GetActorLocation(),
		myplayer->GetActorLocation() + myplayer->GetActorForwardVector() * attack_range,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(attact_radius),
		Params);

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
					enemyActor->MyTakeDamage(myplayer, 10.f, EnemyHitedState::RightLeft, 2.f);
					enemyActor->LandingTimer(); //내가 때리다 말수있으므로 착륙 타이머시킴

					if (airComboCount == 1)
						enemyActor->LaunchCharacter(FVector(0, 0, 1500), false, false);
					else if (airComboCount == 2)
						FixedPositionInTheAir(enemyActor);
				}
				else
				{
					enemyActor->GetCharacterMovement()->GravityScale = 1.f;
					enemyActor->MyTakeDamage(myplayer, 10.f, EnemyHitedState::knock, 3.f, FVector(0, 0, -300.f), TEXT("SWORD_DANCE_READY"));
					enemyActor->LandingTimer_Off();//타이머중인건꺼야지혼선방지
				}
			}
		}
	}

	CheckFinishAttack();

	if (myplayer->GetController() == UGameplayStatics::GetPlayerController(GetWorld(), 0))
		GetWorld()->GetFirstPlayerController()->ClientPlayCameraShake(UMyMatineeCameraShake::StaticClass(), 7.f, ECameraAnimPlaySpace::CameraLocal);

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
					myplayer->GetCharacterMovement()->GravityScale = 0.f;
					myplayer->GetCharacterMovement()->StopMovementImmediately();
					auto* particle3 = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), myplayer->Particle_teleportExit, myplayer->GetActorLocation());
					particle3->CustomTimeDilation = 0.5f;

					Skill_SwordDance();
					myplayer->CustomTimeDilation = 1.f;

				}), WaitTime2, false); 

		}), WaitTime, false); 
}

void UNormalSkill::Skill_SwordWave_Multicast_Implementation()
{
	
	if (!myplayer->GetCharacterMovement()->IsMovingOnGround() || !myplayer->Attack_Melee_StateCheck()) return;
	if (!myplayer->CheckCoolTimeAndStamina(SkillName::swordWave)) return;

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
	auto* wave=  GetWorld()->SpawnActor<AActor>(myplayer->waveActorBP, SpawnLocation , rotator, SpawnParams);

	FString waveName;
	FOutputDeviceNull OutputDeviceNull;
	if(waveCount == LastWaveCount)
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
	float attack_range = 400.f;
	float attact_radius = 400.f;
	TArray<FHitResult> HitResults;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(myplayer);

	bool bResult = GetWorld()->SweepMultiByChannel(
		HitResults,
		myplayer->GetActorLocation(),
		myplayer->GetActorLocation() + myplayer->GetActorForwardVector() * attack_range,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(attact_radius),
		Params);
	
	if (bResult)
	{
		for (const FHitResult& HitResult : HitResults)
		{
			ABasicEnemy* enemyActor = Cast<ABasicEnemy>(HitResult.Actor);
			if (enemyActor && !enemyActor->GetCharacterMovement()->IsMovingOnGround())
			{
				enemyActor->CustomTimeDilation = 1.f;
				enemyActor->MyTakeDamage(myplayer, 10.f, EnemyHitedState::knock, 1.f);
			}	
		}
	}
	if (myplayer->GetController() == UGameplayStatics::GetPlayerController(GetWorld(), 0))
		GetWorld()->GetFirstPlayerController()->ClientPlayCameraShake(UMyMatineeCameraShake::StaticClass(), 20.f, ECameraAnimPlaySpace::CameraLocal);
}

void UNormalSkill::Skill_Enemy_fixed()
{

	float attack_range = 800.f;
	float attact_radius = 800.f;
	TArray<FHitResult> HitResults;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(myplayer);

	bool bResult = GetWorld()->SweepMultiByChannel(
		HitResults,
		myplayer->GetActorLocation(),
		myplayer->GetActorLocation() + myplayer->GetActorForwardVector() * attack_range,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(attact_radius),
		Params);
	
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

	float start = 0.f;
	float radius = 500.f;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(myplayer); //예외 충돌대상

	bool bResult = GetWorld()->SweepMultiByChannel(
		HitResults,
		myplayer->GetActorLocation() + myplayer->GetActorForwardVector() * start,
		myplayer->GetActorLocation() + myplayer->GetActorForwardVector() * radius,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(radius),
		Params);
	
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