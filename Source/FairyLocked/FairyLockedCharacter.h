// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "FairyLockedCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class AFairyLockedCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;

public:

	/** Constructor */
	AFairyLockedCharacter();
	

protected:

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

public:

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
public:
	// --- Gliding Mechanics ---
	
	UFUNCTION(BlueprintCallable, Category = "Gliding")
	void StartGliding();

	UFUNCTION(BlueprintCallable, Category = "Gliding")
	void StopGliding();

	// Tracks if we are currently flying
	UPROPERTY(BlueprintReadOnly, Category = "Gliding State")
	bool bIsGliding = false;

protected:
	// Override Tick to handle the constant flight math
	virtual void Tick(float DeltaTime) override;

	// Overriding Jump to allow double-jumping into a glide
	virtual void Jump() override;

	// --- Flight Tuning Variables ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight Tuning")
	float BaseSinkRate = 150.0f; // How fast you fall when gliding flat

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight Tuning")
	float DiveAcceleration = 2000.0f; // Speed gained when looking down

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight Tuning")
	float LiftMultiplier = 1.5f; // How much speed converts to upward lift

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight Tuning")
	float DragMultiplier = 0.99f; // Air resistance (1.0 is no drag)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight Tuning")
	float ForwardThrust = 3000.0f; // Force applied when holding W
	
	// NEW: Force applied when holding A or D
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight Tuning")
	float LateralThrust = 2000.0f; 

	// NEW: Extra drag applied when holding S to slow down
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight Tuning")
	float AirBrakeDrag = 0.95f;

private:
	// Tracks how hard the player is holding the W key
	float CurrentForwardInput = 0.0f;
	float CurrentRightInput = 0.0f;
};

