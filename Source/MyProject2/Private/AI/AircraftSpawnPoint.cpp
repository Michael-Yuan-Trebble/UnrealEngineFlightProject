// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/AircraftSpawnPoint.h"
#include "Units/Aircraft/BaseAircraft.h"
#include "AI/AircraftAIController.h"
#include "Debug/DebugHelper.h"
#include "Structs and Data/MathLib/FlightMathLibrary.h"

void AAircraftSpawnPoint::ActivateSpawn() 
{
	Super::ActivateSpawn();

	if (bSpawned || !IsValid(UnitClass)) return;

	UWorld* World = GetWorld();

	if (!IsValid(World)) return;

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
			UnitClass,
			SpawnLocation,
			BaseRotation,
			Params
		);

		if (!IsValid(SpawnedAircraft)) continue;
		AliveUnits.Add(SpawnedAircraft);
		SpawnedAircraft->OnDestroyed.AddDynamic(this, &ABaseSpawnPoint::OnUnitDestroyed);

		if (SpawnedAircraft->AutoPossessAI == EAutoPossessAI::Disabled) 
		{
			FActorSpawnParameters AIParams;
			AIParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			AAircraftAIController* AIC = World->SpawnActor<AAircraftAIController>(AAircraftAIController::StaticClass(), FTransform(), AIParams);
			if (!IsValid(AIC)) continue;
			AIC->Possess(SpawnedAircraft);
			SetInitialSpeed(SpawnedAircraft);
		}
	}
	bSpawned = true;
}

void AAircraftSpawnPoint::StressTest() 
{
	if (!IsValid(UnitClass)) return;
	for (int32 i = 0; i < Count; i++)
	{
		FVector Location = GetActorLocation() + FVector(i * 200.f, i * 200.f, 0);
		FRotator Rotation = GetActorRotation();

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		ABaseAircraft* Aircraft = GetWorld()->SpawnActor<ABaseAircraft>(UnitClass, Location, Rotation, Params);
		if (IsValid(Aircraft))
		{
			// Schedule destruction after a short delay
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, [Aircraft]()
				{
					if (IsValid(Aircraft)) Aircraft->Destroy();
				}, 
				DestroyDelay, false);
		}
	}
}

void AAircraftSpawnPoint::SetInitialSpeed(APawn* Spawn) {
	if (!IsValid(Spawn)) return;
	if (ABaseAircraft* Aircraft = Cast<ABaseAircraft>(Spawn)) {
		Aircraft->SetSpeed(UFlightMathLibrary::ConvertKMHToSpeed(InitialSpeed));
	}
}