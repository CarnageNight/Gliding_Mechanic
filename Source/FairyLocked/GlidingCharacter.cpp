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
		ProcessRoll(Rate * 2.f);
}

void AGlidingCharacter::ProcessMouseYInput(float Value)
{
	ProcessPitch(Value);
}

void AGlidingCharacter::ProcessMouseXInput(float Value)
{
	ProcessRoll(Value);
}

void AGlidingCharacter::ProcessRoll(float Value)
{
	bIntentionalRoll = FMath::Abs(Value) > 0.f;
	
	if (bIntentionalPitch && !bIntentionalRoll) return;
	
	const float TargetRollSpeed = bIntentionalRoll ? (Value * RollRateMultiplier) : (GetActorRotation().Roll * -2.f);
	
	CurrentRollSpeed =  FMath::FInterpTo(CurrentRollSpeed, TargetRollSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}

void AGlidingCharacter::ProcessPitch(float Value)
{
	bIntentionalPitch = FMath::Abs(Value) > 0.f;
	
	const float TargetPitchSpeed = Value * PitchRateMultiplier;
	
	CurrentPitchSpeed =  FMath::FInterpTo(CurrentPitchSpeed, TargetPitchSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}

// Called when the game starts or when spawned
void AGlidingCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGlidingCharacter::Tick(float DeltaTime)
{
	
	//TO DO:: make so only apply thrust when holdding W
			
	//Calculate Thrust
	const float CurrentAcc = -GetActorRotation().Pitch * DeltaTime * Acceleration;
	const float NewForwardSpeed = CurrentForwardSpeed + CurrentAcc;
	CurrentForwardSpeed = FMath::Clamp(NewForwardSpeed, MinSpeed, MaxSpeed);
	
	const FVector LocalMove = FVector(CurrentForwardSpeed * DeltaTime, 0.f, 0.f);
	
	AddActorLocalOffset(LocalMove, true);
	
	
	
	FRotator DeltaRotation(0, 0, 0);
	DeltaRotation.Roll = CurrentRollSpeed * DeltaTime;
	DeltaRotation.Yaw = CurrentYawSpeed * DeltaTime;
	DeltaRotation.Pitch = CurrentPitchSpeed * DeltaTime;
	
	AddActorLocalRotation(DeltaRotation);
	
	GEngine -> AddOnScreenDebugMessage(0, 0.f, FColor::Green, FString::Printf(TEXT("CurrentForwardSpeed: %f"), CurrentForwardSpeed));
	
	Super::Tick(DeltaTime);

}

void AGlidingCharacter::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other,
	class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal,
	FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
	
	//Deflects off teh surface
	const FRotator CurrentRotation = GetActorRotation();
	SetActorRotation(FQuat::Slerp(CurrentRotation.Quaternion(), HitNormal.ToOrientationQuat(), .025f));
	
	//Slow down
	CurrentForwardSpeed = FMath::FInterpTo(CurrentForwardSpeed, MinSpeed, GetWorld()->GetDeltaSeconds(), 5.f);
	
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

