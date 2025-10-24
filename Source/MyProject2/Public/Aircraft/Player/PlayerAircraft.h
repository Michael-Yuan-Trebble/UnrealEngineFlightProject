// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aircraft/BaseAircraft.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PlayerAircraft.generated.h"

class AAircraftPlayerController;
class UCameraManagerComponent;

UCLASS()
class MYPROJECT2_API APlayerAircraft : public ABaseAircraft
{
	GENERATED_BODY()
public:
	APlayerAircraft();

	USpringArmComponent* GetSpringArm() const;

	UCameraComponent* GetCamera() const;

	virtual void PossessedBy(AController* Controller) override;

	AAircraftPlayerController* Controlled;

	UCameraManagerComponent* ManagerComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCameraComponent* Camera;

	float ReturnSpringArmLength() const;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
};
