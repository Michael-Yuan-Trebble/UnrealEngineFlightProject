// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Enums/CameraPerspective.h"
#include "CameraManagerComponent.generated.h"

class APlayerAircraft;
class APlayerHUD;
class UAircraftAudioComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT2_API UCameraManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCameraManagerComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetHorizontal(float X);
	void SetVertical(float Y);

	UPROPERTY()
	USpringArmComponent* FirstPersonSpringArm;

	UPROPERTY()
	USpringArmComponent* ThirdPersonSpringArm;

	UPROPERTY()
	USpringArmComponent* MainSpringArm;

	UPROPERTY()
	UAircraftAudioComponent* AudioComp;

	void SetControlled(APlayerAircraft* InControl) { Controlled = InControl; };

	void SetHUD(APlayerHUD* InHUD) { HUD = InHUD; };

	void SetAudioComp(UAircraftAudioComponent* InAudio) { AudioComp = InAudio; };

	void SwitchCamera();

	ECameraPerspective CurrentMode = ECameraPerspective::ThirdPerson;

	UPROPERTY()
	APlayerAircraft* Controlled;

	UPROPERTY()
	APlayerHUD* HUD;

	float LookXLock = 0.f;
	float LookYLock = 0.f;

	float Sensitivity = 1.f;

	float CameraRollOffset = 0.f;
	float TargetRollOffset = 0.f;

	float RollLagStrength = 0.25f;

	float RollLagSpeed = 4.f;

	void SetThirdPerson();
	
	float InterpSpeed = 1.f;

	void SetInterp(float In) { InterpSpeed = In; };

	void SetSpringArm(float Throttle, float D);

protected:
	virtual void BeginPlay() override;
		
private:
	void HandleHorizontal();
	void HandleVertical();

	void HandleRollLag(float Delta);

	float LookX = 0.f;
	float LookY = 0.f;

	bool bXZero = true;
	bool bYZero = true;

	float FirstPersonLength = 0.f;
	float ThirdPersonLength = 0.f;

	void SetFirstPerson();

};
