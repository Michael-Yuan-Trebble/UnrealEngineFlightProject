// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "BaseIRMissile.generated.h"

UCLASS()
class MYPROJECT2_API ABaseIRMissile : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseIRMissile();

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Missile;

	float cooldownTime;

	float timeTilDet;

	float timeDet;

	bool isAir;

	virtual void FireStatic(float launchSpeed);

	virtual void FireTracking(float launchSpeed, AActor* Target);

protected:

	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;

	float ReturnCooldownTime();

};
