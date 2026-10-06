// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GlidingCharacter.generated.h"

UCLASS()
class FAIRYLOCKED_API AGlidingCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGlidingCharacter();
	
	
	UPROPERTY(EditAnywhere, Category = "Flight")
	float MaxSpeed{4000.f};
	
	UPROPERTY(EditAnywhere, Category = "Flight")
	float MaxReverseSpeed{-800.f};
	
	UPROPERTY(EditAnywhere, Category = "Flight")
	float MinSpeed{0.f};
	

	
	UPROPERTY(EditAnywhere, Category = "Flight")
	float PitchRateMultiplier{200.f};
	
	UPROPERTY(EditAnywhere, Category = "Flight")
	float CurrentForwardSpeed{500.f};
	
	UPROPERTY(EditAnywhere, Category = "Flight")
	float DiveAcceleration{30.f};

	UPROPERTY(EditAnywhere, Category = "Flight")
	float ClimbDeceleration{10.f};

	UPROPERTY(EditAnywhere, Category = "Flight")
	float DivePitchThreshold{5.f};

	UPROPERTY(EditAnywhere, Category = "Flight")
	float ClimbPitchThreshold{5.f};
	
	UFUNCTION(BlueprintCallable, Category = "Flight")
	void AddSpeedBoost(float Amount);
	
	
	
	float CurrentYawSpeed;
	float CurrentPitchSpeed;
	
	bool bIntentionalPitch{false};
	
	UPROPERTY(EditAnywhere, Category = "Flight")
	float ThrustAcceleration{2000.f};

	UPROPERTY(EditAnywhere, Category = "Flight")
	float Drag{15.f};

	UPROPERTY(EditAnywhere, Category = "Flight")
	float VerticalThrustSpeed{600.f};
	
	UPROPERTY(EditAnywhere, Category = "Flight")
	float YawRateMultiplier{100.f};

	UPROPERTY(EditAnywhere, Category = "Flight")
	float VisualBankAngle{45.f};

	UPROPERTY(EditAnywhere, Category = "Flight")
	float BankInterpSpeed{5.f};
	
	UPROPERTY(EditAnywhere, Category = "Flight")
	float MaxRollAngle{25.f};

	UPROPERTY(EditAnywhere, Category = "Flight")
	float RollRecoverySpeed{3.f};


protected:
	
	void ProcessYawInput(float Value);
	void ProcessKeyYaw(float Rate);
	float CurrentYawInput{0.f};
	
	void ProcessVerticalThrust(float Value);

	float CurrentVerticalInput{0.f};
	void ProcessKeyPitch(float Rate);
	
	void ProcessMouseYInput(float Rate);
	
	//to calculate rotation
	void ProcessPitch(float Value);
	
	void ProcessThrust(float Value);

	float CurrentThrustInput{0.f};
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
