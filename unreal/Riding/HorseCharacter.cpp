// Fill out your copyright notice in the Description page of Project Settings.


#include "HorseCharacter.h"

// Sets default values
AHorseCharacter::AHorseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void AHorseCharacter::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AHorseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckTiltBody(DeltaTime);
}

// Called to bind functionality to input
void AHorseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForWard", this, &AHorseCharacter::MoveForWard);
	PlayerInputComponent->BindAxis("MoveRight", this, &AHorseCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AHorseCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &AHorseCharacter::AddControllerPitchInput);
	InputComponent->BindAction("Jump", IE_Pressed, this, &AHorseCharacter::StartJump);
}

void AHorseCharacter::MoveForWard(float value)
{
	if (!Controller) return;
	if (value == 0) return;
	if (enable_move == false) return;
	//if (GetVelocity().Size() < 1200 && isRight != 0)  value *= 0.8f;

	// 어느 쪽이 전방인지 알아내어 플레이어가 그 방향으로 이동하고자 한다고 기록합니다.
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, value);
}

void AHorseCharacter::MoveRight(float value)
{
	if (!Controller) return;
	if (value == 0) return;
	if (enable_move == false) return;

	//if (GetVelocity().Size() < 1200 && isRight != 0) value *= 0.8f;

	// 어느 쪽이 오른쪽인지 알아내어 플레이어가 그 방향으로 이동하고자 한다고 기록합니다.
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, value);
}

void AHorseCharacter::StartJump()
{
	if(GetVelocity().Size()>1200)
	bPressedJump = true;
}

void AHorseCharacter::CheckTiltBody(float DeltaTime)
{
	if (oldForward != GetActorForwardVector())
	{
		auto res = FVector::DotProduct(GetActorRightVector(), GetActorForwardVector() - oldForward);
		if (res > 0)//우회전
		{
			if (direction_count < 0.f)
				direction_count = 0.f;

			direction_count += DeltaTime;
			if (direction_count >= 0.2f)
				isRight = 1;
		}
		else if (res < 0)
		{
			if (direction_count > 0.f)
				direction_count = 0.f;

			direction_count -= DeltaTime;
			if (direction_count <= -0.2f)
				isRight = -1;
		}

	}
	else
	{
		direction_count = 0.f;
		isRight = 0;
	}
	oldForward = GetActorForwardVector();
}
