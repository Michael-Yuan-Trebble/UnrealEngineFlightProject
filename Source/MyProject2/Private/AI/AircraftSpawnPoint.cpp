// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("AI Spawned!"));
#include "AI/AircraftSpawnPoint.h"
#include "Aircraft/AI/EnemyAircraftAI.h"

void AAircraftSpawnPoint::BeginPlay() {
	Super::BeginPlay();
	ActivateSpawn();
}

void AAircraftSpawnPoint::ActivateSpawn() 
{
	print(text)
	if (!AircraftClass) return;
	UWorld* World = GetWorld();

	if (!World) return;

	FVector BaseLocation = GetActorLocation();
	FRotator BaseRotation = GetActorRotation();

	for (int32 i = 0; i < Count; i++) {
		FVector Offset = BaseRotation.RotateVector(
			FVector(i * FormationSpacing, 0.f, 0.f)
			);
		FVector SpawnLocation = BaseLocation + Offset;

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		APawn* SpawnedAircraft = World->SpawnActor<APawn>(
			AircraftClass,
			SpawnLocation,
			BaseRotation,
			Params
		);

		if (!SpawnedAircraft) continue;

		if (SpawnedAircraft->AutoPossessAI == EAutoPossessAI::Disabled) {
			AEnemyAircraftAI* AIC = World->SpawnActor<AEnemyAircraftAI>();
			if (AIC) {
				AIC->Possess(SpawnedAircraft);
			}
		}
		UE_LOG(LogTemp, Log, TEXT("Spawned Aircraft %s at %s"),
			*SpawnedAircraft->GetName(),
			*SpawnLocation.ToString());
	}
}