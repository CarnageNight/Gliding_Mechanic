// Fill out your copyright notice in the Description page of Project Settings.


#include "GlidingCharacter.h"

// Sets default values
AGlidingCharacter::AGlidingCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AGlidingCharacter::ProcessKeyPitch(float Rate)
{
	if (FMath::Abs(Rate) > .2f)
		ProcessPitch(Rate * 2.f);
}

void AGlidingCharacter::ProcessKeyRoll(float Rate)
{
	if (FMath::Abs(Rate) > .2f)
		ProcessPitch(Rate * 2.f);
}

void AGlidingCharacter::ProcessMouseYInput(float Rate)
{
}

void AGlidingCharacter::ProcessMouseXInput(float Rate)
{
}

void AGlidingCharacter::ProcessRoll(float Value)
{
}

void AGlidingCharacter::ProcessPitch(float Value)
{
}

// Called when the game starts or when spawned
void AGlidingCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGlidingCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AGlidingCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAxis("Turn", this, &AGlidingCharacter::ProcessMouseXInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AGlidingCharacter::ProcessKeyRoll);
	PlayerInputComponent->BindAxis("LookUp", this, &AGlidingCharacter::ProcessMouseYInput);
	PlayerInputComponent->BindAxis("LookUprate", this, &AGlidingCharacter::ProcessKeyPitch);


}

