// Fill out your copyright notice in the Description page of Project Settings.


#include "GlidingCharacter.h"

// Sets default values
AGlidingCharacter::AGlidingCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AGlidingCharacter::ProcessVerticalThrust(float Value)
{
	CurrentVerticalInput = Value;
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
	
	CurrentRollSpeed =  FMath::FInterpTo(CurrentRollSpeed, TargetRollSpeed, GetWorld()->GetDeltaSeconds(), 8.f);
}

void AGlidingCharacter::ProcessPitch(float Value)
{
	bIntentionalPitch = FMath::Abs(Value) > 0.f;
    
	if (bIntentionalRoll && !bIntentionalPitch) return;
    
	const float TargetPitchSpeed = bIntentionalPitch ? (Value * PitchRateMultiplier) : (GetActorRotation().Pitch * -2.f);
    
	CurrentPitchSpeed = FMath::FInterpTo(CurrentPitchSpeed, TargetPitchSpeed, GetWorld()->GetDeltaSeconds(), 8.f);
}

void AGlidingCharacter::ProcessThrust(float Value)
{
	CurrentThrustInput = Value;
}

// Called when the game starts or when spawned
void AGlidingCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGlidingCharacter::Tick(float DeltaTime)
{
	
	// Calculate Thrust
	const float Pitch = GetActorRotation().Pitch;

	float GravityAcc = 0.f;
	if (Pitch < -DivePitchThreshold)
	{
		// Past the dive threshold — scale from where the threshold ends, not from zero
		GravityAcc = (-Pitch - DivePitchThreshold) * DeltaTime * DiveAcceleration;
	}
	else if (Pitch > ClimbPitchThreshold)
	{
		GravityAcc = -(Pitch - ClimbPitchThreshold) * DeltaTime * ClimbDeceleration;
	}
	const float ThrustAcc = CurrentThrustInput * ThrustAcceleration * DeltaTime;

	// Passive drag pulls speed back toward MinSpeed when not actively thrusting.
	const bool bThrusting = FMath::Abs(CurrentThrustInput) > KINDA_SMALL_NUMBER;
	const float DragAcc = bThrusting ? 0.f : (MinSpeed - CurrentForwardSpeed) * Drag * DeltaTime;

	const float NewForwardSpeed = CurrentForwardSpeed + GravityAcc + ThrustAcc + DragAcc;
	
	const bool bReversing = CurrentThrustInput < -KINDA_SMALL_NUMBER;
	const float MinBound = bReversing ? MaxReverseSpeed : MinSpeed;

	CurrentForwardSpeed = FMath::Clamp(NewForwardSpeed, MinBound, MaxSpeed);
	
	const FVector LocalMove = FVector(CurrentForwardSpeed * DeltaTime, 0.f, 0.f);
	AddActorLocalOffset(LocalMove, true);

	const FVector VerticalMove = FVector(0.f, 0.f, CurrentVerticalInput * VerticalThrustSpeed * DeltaTime);
	AddActorWorldOffset(VerticalMove, true);
	
	
	
	const float CurrentRoll = GetActorRotation().Roll;
	CurrentYawSpeed = CurrentRoll * TurnRateFromRoll;
	
	float RollDelta = CurrentRollSpeed * DeltaTime;
	

	const float ProjectedRoll = FMath::ClampAngle(CurrentRoll + RollDelta, -MaxRollAngle, MaxRollAngle);
	RollDelta = ProjectedRoll - CurrentRoll;

	AddActorLocalRotation(FRotator(0.f, 0.f, RollDelta));
	AddActorLocalRotation(FRotator(CurrentPitchSpeed * DeltaTime, 0.f, 0.f));
	AddActorLocalRotation(FRotator(0.f, CurrentYawSpeed * DeltaTime, 0.f));


	
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
	PlayerInputComponent->BindAxis("MoveForward", this, &AGlidingCharacter::ProcessThrust);
	PlayerInputComponent->BindAxis("MoveUp", this, &AGlidingCharacter::ProcessVerticalThrust);


}

