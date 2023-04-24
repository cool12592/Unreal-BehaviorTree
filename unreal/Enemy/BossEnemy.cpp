// Fill out your copyright notice in the Description page of Project Settings.


#include "BossEnemy.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Misc/OutputDeviceNull.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Engine.h"
#include "Kismet/KismetMathLibrary.h"
#include "PlayerCharacter.h"
#include "Components/WidgetComponent.h"
#include "MyMatineeCameraShake.h"
ABossEnemy::ABossEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABossEnemy::BeginPlay()
{
	Super::BeginPlay();
	detectRange = 10000.f;
	attackRange = 1500.f;
	TrackToRange = 400.F;

	isBoss = true;
	attackCoolTime = 2.f;
	backAttackCoolTime = 2.f;
	suddenAttackCoolTime = 2.f;
	TurnCoolTime = 2.f;

	MaxHP = 120.f;
	HP = MaxHP;
}

void ABossEnemy::Tick(float DeltaTime)
{
	RunCoolTime(DeltaTime);
	TickParabola(DeltaTime);
	RotationArroundToPlayer();

	if (IsAttacking) return;
	CalculateDistFromPlayer();
	CheckBackAttack();
	CheckSuddenAttack();
	Turn();

}

void ABossEnemy::RunCoolTime(float DeltaTime)
{
	TurnCoolTime -= DeltaTime;
	attackCoolTime -= DeltaTime;
	backAttackCoolTime -= DeltaTime;
	suddenAttackCoolTime -= DeltaTime;
}

void ABossEnemy::CalculateDistFromPlayer()
{
	startLocation = GetActorLocation();
	targetLocation = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation();
	distance = (targetLocation - startLocation).Size();
}

void ABossEnemy::CheckSuddenAttack()
{
	if (suddenAttackCoolTime <= 0.f) //공격중은아니고 쿨기다릴때
	{
		if (distance <= 600.f)
		{
			int rnd = FMath::RandRange(1, 10);
			if (rnd <= 3)
			{
				suddenAttackCoolTime = 5.f;
				attackCoolTime = 2.f;
				IsAttacking = true;
				Attack3_Multicast(); //얘는 애니메이션자체에서 움직임
			}
			else
				suddenAttackCoolTime = 0.9f; //한번은더할수있게
		}
		return;
	}
}

void ABossEnemy::CheckBackAttack()
{
	if (backAttackCoolTime <= 0.f)
	{
		if (distance <= 600.f && FVector::DotProduct(GetActorForwardVector(), (targetLocation - startLocation).GetSafeNormal()) <= -0.4f)
		{
			int rnd = FMath::RandRange(1, 10);
			if (rnd <= 2)
			{
				backAttackCoolTime = 3.f;
				return;
			}

			IsAttacking = true;
			attackCoolTime = 2.f;
			backAttackCoolTime = 5.f;
			Attack5_Multicast();
		}
	}
}

void ABossEnemy::Turn()
{
	if (TurnCoolTime > 0) return;
	auto Dot = FVector::DotProduct((targetLocation - startLocation).GetSafeNormal(), GetActorRightVector());
	if (Dot >= 0.7f)//우회전
	{
		TurnCoolTime = 1.f;
		PlayAnimMontage(RTurnAnim, 1.f);
	}
	else if (Dot <= -0.7f)
	{
		TurnCoolTime = 1.f;
		PlayAnimMontage(LTurnAnim, 1.f);
	}
}

void ABossEnemy::Attack()
{
	if (IsAttacking || isDie) return;

	CalculateDistFromPlayer();

	int attackNum;
	if (distance <= 900.f)
		attackNum= FMath::RandRange(1, 3);
	else
		attackNum = FMath::RandRange(4, 5);
	
	if (firstAttack)
	{
		firstAttack = false;
		attackNum = 5;
	}

	attackCoolTime = 3.f;
	IsAttacking = true;

	if (attackNum == 1 )
		Attack1_Multicast();
	else if (attackNum == 2)
		Attack1_Multicast();
	else if (attackNum == 3)
		Attack3_Multicast();
	else if (attackNum == 4)
		Attack2_Multicast();
	else if (attackNum == 5 )
		Attack4_Multicast();
	else
		return;
}

void ABossEnemy::Attack5_Multicast_Implementation()
{
	SetActorRotation((GetActorForwardVector() * -1.f).Rotation());
	PlayAnimMontage(attackAnim5, 1.f);
}

void ABossEnemy::Attack1_Multicast_Implementation()
{
	PlayAnimMontage(attackAnim1, 1.f);
}

void ABossEnemy::Attack2_Multicast_Implementation()
{
	PlayAnimMontage(attackAnim2, 1.f);
}

void ABossEnemy::Attack3_Multicast_Implementation()
{
	PlayAnimMontage(attackAnim3, 1.f);
}

void ABossEnemy::Attack4_Multicast_Implementation()
{
	isRotationArroundToPlayer = true;

	int attackNum = FMath::RandRange(1, 2);
	if(attackNum==1)
		PlayAnimMontage(LDodgeAnim, 1.f);
	else
		PlayAnimMontage(RDodgeAnim, 1.f);
}

void ABossEnemy::RotationArroundToPlayer()
{
	if (isRotationArroundToPlayer)
	{
		CalculateDistFromPlayer();
		auto rotation = UKismetMathLibrary::FindLookAtRotation(startLocation, targetLocation);
		auto interpRotation = FMath::RInterpTo(GetActorRotation(), rotation, GetWorld()->GetDeltaSeconds(), 10.f);

		SetActorRotation(FRotator(GetActorRotation().Pitch, interpRotation.Yaw, GetActorRotation().Roll));
	}

}
void ABossEnemy::LaunchToPlayer()
{
	GetCharacterMovement()->StopMovementImmediately();
	PlayAnimMontage(attackAnim4, 1.f);

	ProjectToTarget();
	isCheckNearGround = false;
	isParabola = true;

	FTimerHandle WaitHandle;
	float WaitTime = 0.5f; 
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{

			isCheckNearGround = true;
		}), WaitTime, false); 
}

void ABossEnemy::ProjectToTarget()
{
	CalculateDistFromPlayer();
	FVector outVelocity = FVector::ZeroVector;   // 결과 Velocity
	float speed = 3000.f;

	UGameplayStatics::SuggestProjectileVelocity(this, outVelocity, startLocation, targetLocation, speed, false,
		GetCapsuleComponent()->GetScaledCapsuleRadius(),
		0.f, ESuggestProjVelocityTraceOption::DoNotTrace);

	float alpha = (targetLocation - startLocation).Size() / speed;
	LaunchCharacter(outVelocity * alpha, true, true);
}

void ABossEnemy::TickParabola(float delta)
{
	if (!isParabola) return;
	float power = 50.f;
	accumulate_down_vertical_force +=  gravitational_acceleration * delta * power; //누적중력 += 중력가속도
	//초기세로힘에서 값이 점점 줄어들면서 포물선을 이루게 됨
	float resultOffset = (vertical_force - accumulate_down_vertical_force) * delta * power;//초기세로힘- 누적중력
	AddActorLocalOffset(FVector(0.F, 0.F, resultOffset)); 

	if (isCheckNearGround)
	{
		if (CheckNearGround())
		{
			isParabola = false;
			accumulate_down_vertical_force = 0.f;
		}
	}
}

bool ABossEnemy::CheckNearGround()
{
	float range = 150.f;
	TArray<FHitResult> HitResults;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bResult = GetWorld()->LineTraceMultiByChannel(HitResults, GetActorLocation(), GetActorLocation() - GetActorUpVector() * range, ECollisionChannel::ECC_GameTraceChannel1, Params);

	if (bResult)
		return true;
	
	return false;
}

void ABossEnemy::BossDie()
{
	isDie = true;
	IsAttacking = true; //동작못하게
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.3f);

	PlayAnimMontage(DieAnim, 1.f);
	FOutputDeviceNull pAR3;
	this->CallFunctionByNameWithArguments(TEXT("BossDieCamera"), pAR3, nullptr, true);
	GetWorld()->GetFirstPlayerController()->ClientPlayCameraShake(UMyMatineeCameraShake::StaticClass(), 5.f, ECameraAnimPlaySpace::CameraLocal);

	FTimerHandle WaitHandle;
	float WaitTime = 8.f;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			FName path = TEXT("Blueprint'/Game/My__/PLAYER/Entrance/ToBoss2_Portal_BP.ToBoss2_Portal_BP_C'");
			UClass* GeneratedBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));

			FVector location = GetActorLocation() + GetActorUpVector() * 200.f;
			FRotator rotation = GetActorRotation();
			GetWorld()->SpawnActor<AActor>(GeneratedBP, location, rotation);

		}), WaitTime, false);
}

void ABossEnemy::Boss_AttackCheck()
{
	float attack_range = 350.f;
	float attact_radius = 350.f;
	TArray<FHitResult> HitResults;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bResult = GetWorld()->SweepMultiByChannel(
		HitResults,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * attack_range,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(attact_radius),
		Params);

	if (bResult)
	{
		for (const FHitResult& HitResult : HitResults)
		{
			APlayerCharacter* player = Cast<APlayerCharacter>(HitResult.Actor);
			if (player)
			{
				player->MyTakeDamage2(15.f, HitedState::knock, 0.f, GetActorForwardVector() * 1500.f +GetActorUpVector()*500.f);
				return;
			}
		}
	}

}


void ABossEnemy::MyTakeDamage(AActor* attacker, float damage, EnemyHitedState hit, float hitedTime_, FVector launchVec, FName note)
{
	if (isDie)
		return;

	if (note == TEXT("meleeReady"))
		return;

	FOutputDeviceNull pAR;
	this->CallFunctionByNameWithArguments(TEXT("BossHit"), pAR, nullptr, true);

	HP -= damage;

	if (HPBarWidget)
		ChangeHP_UI();
	
	if (HP <= 0)
		BossDie();
}