// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Structs and Data/Weapon Data/BulletStats.h"
#include "AircraftStats.generated.h"

UCLASS()
class MYPROJECT2_API UAircraftStats : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName AircraftName;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float MaxSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Acceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TurnRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AOARecoverySpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RudderRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RollRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpringArmLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int NumOfPylons;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* Pylon;
};
