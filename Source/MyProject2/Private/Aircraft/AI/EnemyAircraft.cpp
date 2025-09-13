// Fill out your copyright notice in the Description page of Project Settings.


#include "Aircraft/AI/EnemyAircraft.h"
#include "Aircraft/AI/EnemyAircraftAI.h"
#include "RadarComponent.h"
#include "Kismet/GameplayStatics.h"
#include "FlightComponent.h"
#include "Aircraft/AI/AIFlightComponent.h"

AEnemyAircraft::AEnemyAircraft() 
{
	FlightComponent = CreateDefaultSubobject<UFlightComponent>(TEXT("FlightComponent"));
	Radar = CreateDefaultSubobject<URadarComponent>(TEXT("Radar"));
}


void AEnemyAircraft::PossessedBy(AController* NewController) {
	Super::PossessedBy(NewController);

	Controller = Cast<AEnemyAircraftAI>(NewController);

	// Debug capsule for radar

	if (Collision)
	{
		FVector CapsuleLocation = Collision->GetComponentLocation();
		FRotator CapsuleRotation = Collision->GetComponentRotation();

		DrawDebugCapsule(
			GetWorld(),
			CapsuleLocation,
			100,
			100,
			CapsuleRotation.Quaternion(),
			FColor::Green,
			false,
			500.0f
		);
	}

	
}

/*
	if (!Targeting.CurrentPawn || (Targeting.CurrentPawn && Targeting.threatLevel < 0.5))
	{
		PickTarget();
	}
*/

// Iterate through All Aircraft in range
// Store the largest threat and make it the one targeted

void AEnemyAircraft::PickTarget() 
{
	for (int i = 0; i < DetectedTargets.Num(); i++)
	{
		if (Targeting.CurrentPawn)
		{
			if (Targeting.threatLevel < DetectedTargets[i].threatLevel)
			{
				Targeting = DetectedTargets[i];
			}
		}
		else
		{
			Targeting = DetectedTargets[i];
		}
	}
}