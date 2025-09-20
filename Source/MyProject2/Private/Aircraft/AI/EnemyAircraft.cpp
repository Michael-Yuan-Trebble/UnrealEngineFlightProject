// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Enemy Stats!"));
#include "Aircraft/AI/EnemyAircraft.h"
#include "Aircraft/AI/EnemyAircraftAI.h"
#include "Aircraft/RadarComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Structs and Data/Aircraft Data/AircraftStats.h"
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
	
	UAircraftStats* Temp = NewObject<UAircraftStats>(this);
	Temp->TurnRate = 0.3;
	Temp->RollRate = 10;
	FlightComponent->AircraftStats = Temp;
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