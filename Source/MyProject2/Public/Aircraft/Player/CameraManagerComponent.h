// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "CameraManagerComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT2_API UCameraManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCameraManagerComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void LookHor(float InputX);

	void LookVer(float InputY);

	USpringArmComponent* SpringArm;

	void SetSpringArm(USpringArmComponent* InArm) { SpringArm = InArm; }

protected:
	virtual void BeginPlay() override;
		
private:
	float currentX = 0.f;

	float currentY = 0.f;

	float prevX = 0.f;

	float prevY = 0.f;
};
