// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("AI!"));
#include "F16AI.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "BaseAircraft.h"
#include "Components/SphereComponent.h"

//Initialize F16AI
AF16AI::AF16AI()
{
	DetectionZone = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionZone"));
	RootComponent = DetectionZone;

	Airframe = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Airframe"));
	Airframe->SetupAttachment(DetectionZone);

	PrimaryActorTick.bCanEverTick = true;
}

void AF16AI::BeginPlay()
{
	Super::BeginPlay();
	
	//Radar only triggers 0.5 seconds at a time
	GetWorldTimerManager().SetTimer(RadarScanTimer, this, &AF16AI::ScanForTargets, 0.5f, true);
}

void AF16AI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AF16AI::ScanForTargets() 
{
	DetectedTargets.Empty();
	TArray<AActor*> AllAircraft;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseAircraft::StaticClass(), AllAircraft);

	//This only detects the Player aircraft rn
	for (AActor* Target : AllAircraft) 
	{
		
	}
}

//Animation (Not yet)

/*

	void PitchElevatorCalculation(float DeltaSeconds);

	void RollElevatorCalculation(float DeltaSeconds);

	void SlatsElevationCalculation(float DeltaSeconds);

	void RudderYawCalculation(float DeltaSeconds);

	void FlapPitchCalculation(float DeltaSeconds);
	*/