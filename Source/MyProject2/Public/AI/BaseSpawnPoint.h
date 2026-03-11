// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enums/TargetTypes.h"
#include "BaseSpawnPoint.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpawnDestroyed, const ABaseSpawnPoint*, SpawnPoint);

class ABaseUnit;

UCLASS()
class MYPROJECT2_API ABaseSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	

	FOnSpawnDestroyed OnWaveDestroyed;

	UFUNCTION(BlueprintCallable)
	virtual void ActivateSpawn();

	virtual void StressTest();

	UFUNCTION()
	void OnUnitDestroyed(AActor* DestroyedActor);

	UPROPERTY(EditAnywhere)
	bool bStressTest = false;

protected:

	UPROPERTY(EditAnywhere)
	TSubclassOf<APawn> UnitClass = nullptr;

	// Idea: When a spawn point gets too low, the units enter "panic" or some other mode and become more agressive, probably just for aircraft though
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> AliveUnits{};

	UPROPERTY(EditAnywhere)
	int32 Count = 1;

	UPROPERTY()
	bool bSpawned = false;

	UPROPERTY(EditAnywhere)
	ETargetType TeamType = ETargetType::Unknown;
};
