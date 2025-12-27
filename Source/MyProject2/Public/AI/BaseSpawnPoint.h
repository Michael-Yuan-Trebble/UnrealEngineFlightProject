// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enums/TargetTypes.h"
#include "BaseSpawnPoint.generated.h"

UCLASS()
class MYPROJECT2_API ABaseSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditAnywhere)
	ETargetType TeamType;

	UFUNCTION(BlueprintCallable)
	virtual void ActivateSpawn();
};
