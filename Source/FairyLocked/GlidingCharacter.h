// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GlidingCharacter.generated.h"

UCLASS()
class FAIRYLOCKED_API AGlidingCharacter : public ACharacter
{
	GENERATED_BODY()
	
	float Acceleration{30.f};
	float MaxSpeed{4000.f};
	float MinSpeed{500.f};
	
	float CurrentForwardSpeed{500.f};
	
	float CurrentYawSpeed;
	float CurrentPitchSpeed;
	float CurrentRollSpeed;

public:
	// Sets default values for this character's properties
	AGlidingCharacter();

protected:
	void ProcessKeyPitch(float Rate);
	void ProcessKeyRoll(float Rate);
	
	void ProcessMouseYInput(float Rate);
	void ProcessMouseXInput(float Rate);
	
	//to calculate rotation
	void ProcessRoll(float Value);
	void ProcessPitch(float Value);
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
