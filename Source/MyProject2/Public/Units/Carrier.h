// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Units/BaseNaval.h"
#include "Carrier.generated.h"

UCLASS()
class MYPROJECT2_API ACarrier : public ABaseNaval
{
	GENERATED_BODY()
	
public:

	ACarrier();

	FTransform GetCarrierSpawnPoint() const { return SpawnPoint->GetComponentTransform(); };

private:
	UPROPERTY(EditAnywhere)
	USceneComponent* SpawnPoint = nullptr;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* CarrierMesh = nullptr;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Collision = nullptr;
};
