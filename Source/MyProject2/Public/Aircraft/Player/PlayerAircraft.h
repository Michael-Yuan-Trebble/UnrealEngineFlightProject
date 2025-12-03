// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aircraft/BaseAircraft.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PlayerAircraft.generated.h"

class AAircraftPlayerController;
class UCameraManagerComponent;
class UAircraftAudioComponent;

UCLASS()
class MYPROJECT2_API APlayerAircraft : public ABaseAircraft
{
	GENERATED_BODY()
public:
	APlayerAircraft();

	virtual void PossessedBy(AController* Controller) override;

	AAircraftPlayerController* Controlled;

	UCameraManagerComponent* ManagerComp;

	UPROPERTY(EditAnywhere)
	UAircraftAudioComponent* AudioComp;

	UAudioComponent* PersonalAircraftAudio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCameraComponent* ThirdPersonCamera;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCameraComponent* FirstPersonCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USpringArmComponent* FirstPersonSpringArm;

	USpringArmComponent* GetSpringArm() const { return SpringArm; };

	UCameraComponent* GetThirdPersonCamera() const { return ThirdPersonCamera; };

	UCameraComponent* GetFirstPersonCamera() const{ return FirstPersonCamera; };

	float ReturnSpringArmLength() const { return springArmLength; };

	UAudioComponent* GetAudioComponent() const { return PersonalAircraftAudio; };

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

private:
	UFUNCTION()
	void HandleHit();
};
