// Fill out your copyright notice in the Description page of Project Settings.


#include "GlidingCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AGlidingCharacter::AGlidingCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->bOrientRotationToMovement = false;
		MoveComp->GravityScale = 0.f;
		MoveComp->SetMovementMode(MOVE_Flying);
	}

}

void AGlidingCharacter::ProcessYawInput(float Value)
{
	CurrentYawInput = Value;
	CurrentYawSpeed = Value * YawRateMultiplier;
}

void AGlidingCharacter::ProcessKeyYaw(float Rate)
{
	if (FMath::Abs(Rate) > .2f)
		ProcessYawInput(Rate * 2.f);
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


void AGlidingCharacter::ProcessMouseYInput(float Value)
{
	ProcessPitch(Value);
}


void AGlidingCharacter::ProcessPitch(float Value)
{
	bIntentionalPitch = FMath::Abs(Value) > 0.f;
	
    
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
	
	
	AddActorLocalRotation(FRotator(CurrentPitchSpeed * DeltaTime, 0.f, 0.f));
	AddActorLocalRotation(FRotator(0.f, CurrentYawSpeed * DeltaTime, 0.f));

	FRotator CurrentRot = GetActorRotation();

	const bool bTurning = FMath::Abs(CurrentYawInput) > KINDA_SMALL_NUMBER;

	if (bTurning)
	{
		// Let roll drift naturally from the turn, just don't let it exceed the limit
		CurrentRot.Roll = FMath::ClampAngle(CurrentRot.Roll, -MaxRollAngle, MaxRollAngle);
	}
	else
	{
		// No turn input — smoothly settle back to level
		CurrentRot.Roll = FMath::FInterpTo(CurrentRot.Roll, 0.f, DeltaTime, RollRecoverySpeed);
	}

	SetActorRotation(CurrentRot);
	
	


	
	GEngine -> AddOnScreenDebugMessage(0, 0.f, FColor::Green, FString::Printf(TEXT("CurrentForwardSpeed: %f"), CurrentForwardSpeed));
	
	GEngine -> AddOnScreenDebugMessage(1, 0.f, FColor::Green, FString::Printf(TEXT("CurrenRotation: %f"), GetActorRotation().Roll));
	
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
	
	PlayerInputComponent->BindAxis("Turn", this, &AGlidingCharacter::ProcessYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AGlidingCharacter::ProcessKeyYaw);
	PlayerInputComponent->BindAxis("LookUp", this, &AGlidingCharacter::ProcessMouseYInput);
	PlayerInputComponent->BindAxis("LookUprate", this, &AGlidingCharacter::ProcessKeyPitch);
	PlayerInputComponent->BindAxis("MoveForward", this, &AGlidingCharacter::ProcessThrust);
	PlayerInputComponent->BindAxis("MoveUp", this, &AGlidingCharacter::ProcessVerticalThrust);


}

