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
	FlightComponent->Controlled = this;
	
	UAircraftStats* Temp = NewObject<UAircraftStats>(this);
	Temp->TurnRate = 0.3;
	Temp->RollRate = 10;
	Temp->Thrust = 10;
	Temp->MaxSpeed = 1000;
	FlightComponent->AircraftStats = Temp;
}