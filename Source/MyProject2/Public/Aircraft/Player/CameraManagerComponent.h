// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "CameraManagerComponent.generated.h"

class APlayerAircraft;

enum class ECameraMode {
	FirstPerson,
	ThirdPerson
};

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

	USpringArmComponent* SpringArm;

	void SetSpringArm(USpringArmComponent* InArm) { SpringArm = InArm; }

	void SetControlled(APlayerAircraft* InControl) { Controlled = InControl; };

	void SwitchCamera();

	ECameraMode CurrentMode = ECameraMode::ThirdPerson;

	APlayerAircraft* Controlled;

	float LookXLock = 0.f;
	float LookYLock = 0.f;

protected:
	virtual void BeginPlay() override;
		
private:
	float currentX = 0.f;

	float currentY = 0.f;

	float FirstPersonX = 0.f;
	float FirstPersonY = 0.f;

	float prevX = 0.f;

	float prevY = 0.f;

	void SetFirstPerson();

	void SetThirdPerson();
};
