// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/SpringArmComponent.h"
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

	void LookHor(float InputX);

	void LookVer(float InputY);

	void ThirdPersonHorizontal(float X);
	void ThirdPersonVertical(float Y);

	void FirstPersonHorizontal(float X);
	void FirstPersonVertical(float Y);

	UPROPERTY()
	USpringArmComponent* SpringArm;

	UPROPERTY()
	UAircraftAudioComponent* AudioComp;

	void SetSpringArm(USpringArmComponent* InArm) { SpringArm = InArm; }

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

	void SetThirdPerson();

protected:
	virtual void BeginPlay() override;
		
private:
	float currentX = 0.f;

	float currentY = 0.f;

	float FirstPersonX = 0.f;
	float FirstPersonY = 0.f;

	float FirstPersonPrevX = 0.f;
	float FirstPersonPrevY = 0.f;

	float prevX = 0.f;

	float prevY = 0.f;

	void SetFirstPerson();

};
