// Fill out your copyright notice in the Description page of Project Settings.


#include "Aircraft/AI/EnemyAircraft.h"
#include "Aircraft/AI/EnemyAircraftAI.h"
#include "Aircraft/RadarComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Aircraft/FlightComponent.h"
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
	FlightComponent->Controlled = this;
	//FlightComponent->AircraftStats
	
}

/*
	if (!Targeting.CurrentPawn || (Targeting.CurrentPawn && Targeting.threatLevel < 0.5))
	{
		PickTarget();
	}
*/

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