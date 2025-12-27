// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("AI Spawned!"));
#include "AI/AircraftSpawnPoint.h"
#include "Units/Aircraft/BaseAircraft.h"
#include "Units/Aircraft/AI/EnemyAircraftAI.h"

void AAircraftSpawnPoint::BeginPlay() 
{
	Super::BeginPlay();
	if (bStressTest) 
	{
		StressTest();
	}
	else {
		ActivateSpawn();
	}
}

void AAircraftSpawnPoint::ActivateSpawn() 
{
	if (bSpawned || !AircraftClass) return;

	UWorld* World = GetWorld();

	if (!World) return;

	FVector BaseLocation = GetActorLocation();
	FRotator BaseRotation = GetActorRotation();

	for (int32 i = 0; i < Count; i++) 
	{
		FVector Offset = BaseRotation.RotateVector(
			FVector(i * FormationSpacing, 0.f, 0.f)
			);
		FVector SpawnLocation = BaseLocation + Offset;

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		APawn* SpawnedAircraft = World->SpawnActor<APawn>(
			AircraftClass,
			SpawnLocation,
			BaseRotation,
			Params
		);

		if (SpawnedAircraft) {
			bSpawned = true;
		}
		else {
			continue;
		}

		if (SpawnedAircraft->AutoPossessAI == EAutoPossessAI::Disabled) 
		{
			FActorSpawnParameters AIParams;
			AIParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			AEnemyAircraftAI* AIC = World->SpawnActor<AEnemyAircraftAI>(AEnemyAircraftAI::StaticClass(), FTransform(), AIParams);
			if (!AIC) continue;
			AIC->Possess(SpawnedAircraft);
		}
	}
}

void AAircraftSpawnPoint::StressTest() 
{
	if (!AircraftClass) return;
	for (int32 i = 0; i < Count; i++)
	{
		FVector Location = GetActorLocation() + FVector(i * 200.f, 0, 0);
		FRotator Rotation = GetActorRotation();

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		ABaseAircraft* Aircraft = GetWorld()->SpawnActor<ABaseAircraft>(AircraftClass, Location, Rotation, Params);
		if (IsValid(Aircraft))
		{
			// Schedule destruction after a short delay
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, [Aircraft]()
				{
					if (IsValid(Aircraft))
					{
						Aircraft->Destroy();
					}
				}, DestroyDelay, false);
		}
	}
}