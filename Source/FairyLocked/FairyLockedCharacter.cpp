// Copyright Epic Games, Inc. All Rights Reserved.

#include "FairyLockedCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "FairyLocked.h"

AFairyLockedCharacter::AFairyLockedCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AFairyLockedCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFairyLockedCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AFairyLockedCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFairyLockedCharacter::Look);
	}
	else
	{
		UE_LOG(LogFairyLocked, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AFairyLockedCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AFairyLockedCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AFairyLockedCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		if (bIsGliding)
		{
			// Store the forward input (W/S) for the Tick function to use
			CurrentForwardInput = FMath::Clamp(Forward, -1.0f, 1.0f);
			CurrentRightInput = FMath::Clamp(Right, -1.0f, 1.0f);
		}
		else
		{
			// find out which way is forward
			const FRotator Rotation = GetController()->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get forward vector
			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

			// get right vector 
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			// add movement 
			AddMovementInput(ForwardDirection, Forward);
			AddMovementInput(RightDirection, Right);
		}
	}
}

void AFairyLockedCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AFairyLockedCharacter::Jump()
{
	// 1. If currently gliding, pressing jump cancels flight
	if (bIsGliding)
	{
		StopGliding();
		return;
	}

	// 2. If in mid-air, pressing jump triggers Gliding
	if (GetCharacterMovement()->IsFalling())
	{
		StartGliding();
	}
	else
	{
		// 3. On the ground -> Standard Jump
		Super::Jump();
	}
}

void AFairyLockedCharacter::DoJumpStart()
{
	Jump();
}

void AFairyLockedCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

void AFairyLockedCharacter::StartGliding()
{
	if (GetCharacterMovement() && GetCharacterMovement()->IsFalling())
	{
		bIsGliding = true;
       
		// Turn off normal gravity
		GetCharacterMovement()->GravityScale = 0.0f; 
       
		// NEW: Turn off Unreal's built-in air brakes so our custom thrust works!
		GetCharacterMovement()->BrakingDecelerationFalling = 0.0f; 
	}
}

void AFairyLockedCharacter::StopGliding()
{ 
	bIsGliding = false;
	if (GetCharacterMovement())
	{
		// Restore normal gravity
		GetCharacterMovement()->GravityScale = 1.0f; 
       
		// NEW: Turn Unreal's air brakes back on for normal jumping
		GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f; 
	}
}

void AFairyLockedCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // If we are on the ground, reset gliding state
    if (bIsGliding && !GetCharacterMovement()->IsFalling())
    {
       StopGliding();
    }

    if (!bIsGliding || GetController() == nullptr) return;

    FRotator ControlRot = GetController()->GetControlRotation();
    FVector CurrentVelocity = GetCharacterMovement()->Velocity;
    
    // Get Camera Vectors
    FVector CameraForwardVector = ControlRot.Vector();
    const FRotator YawRotation(0, ControlRot.Yaw, 0);
    FVector CameraRightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    // --- 1. STEERING (Fixes the camera direction bug!) ---
    // Smoothly bend our existing momentum towards where the camera is looking
    float CurrentSpeed = CurrentVelocity.Size();
    FVector CurrentDir = CurrentVelocity.GetSafeNormal();
    
    // 3.0f is the turn speed. Higher number = sharper turns.
    FVector NewDir = FMath::VInterpTo(CurrentDir, CameraForwardVector, DeltaTime, 3.0f).GetSafeNormal();
    CurrentVelocity = NewDir * CurrentSpeed;

    // --- 2. DIVE & LIFT PHYSICS ---
    // ONLY apply natural Glider physics if we are NOT holding W
    if (CurrentForwardInput <= 0.0f)
    {
       float Pitch = ControlRot.Pitch; 
       float PitchRatio = FMath::Clamp(Pitch / 90.0f, -1.0f, 1.0f); 

       if (PitchRatio < 0.0f) // Looking down
       {
          CurrentVelocity += CameraForwardVector * (FMath::Abs(PitchRatio) * DiveAcceleration * DeltaTime);
       }
       else if (PitchRatio > 0.0f) // Looking up (stalling)
       {
          float ForwardSpeed = FVector::DotProduct(CurrentVelocity, CameraForwardVector);
          ForwardSpeed = FMath::Max(0.0f, ForwardSpeed); 
          
          CurrentVelocity *= FMath::Max(0.0f, 1.0f - (PitchRatio * DeltaTime)); 
          CurrentVelocity.Z += ForwardSpeed * PitchRatio * LiftMultiplier * DeltaTime; 
       }

       // Normal gravity sink rate
       CurrentVelocity.Z -= BaseSinkRate * DeltaTime;
    }

    // --- 3. FORWARD THRUST (W and S) ---
    if (CurrentForwardInput > 0.0f)
    {
       // Flatten the vector so W only pushes horizontally
       FVector FlatForwardVector = CameraForwardVector;
       FlatForwardVector.Z = 0.0f; 
       FlatForwardVector.Normalize();
       
       CurrentVelocity += FlatForwardVector * (CurrentForwardInput * ForwardThrust * DeltaTime);
       
       // PREVENT UPWARD FLIGHT: Force Z to not exceed 0 while holding W
       if (CurrentVelocity.Z > 0.0f)
       {
           CurrentVelocity.Z = 0.0f;
       }
    }
    else if (CurrentForwardInput < 0.0f)
    {
       // S: Airbrakes
       CurrentVelocity *= AirBrakeDrag;
    }

    // --- 4. LATERAL MOVEMENT (A and D) ---
    if (CurrentRightInput > 0.1f) 
    {
       // Pressing D: Full speed Right
       CurrentVelocity += CameraRightVector * (LateralThrust * DeltaTime);
    }
    else if (CurrentRightInput < -0.1f) 
    {
       // Pressing A: Full speed Left
       CurrentVelocity += CameraRightVector * (-LateralThrust * DeltaTime);
    }

    // --- 5. DRAG ---
    CurrentVelocity *= DragMultiplier; 

    // Apply final velocity to character
    GetCharacterMovement()->Velocity = CurrentVelocity;

    // Reset inputs each frame
    CurrentForwardInput = 0.0f; 
    CurrentRightInput = 0.0f; 
}