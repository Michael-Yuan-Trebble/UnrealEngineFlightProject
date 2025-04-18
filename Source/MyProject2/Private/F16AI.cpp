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

	AIMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("AIMesh"));
	RootComponent = AIMesh;

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
	//Actively scans for all pawns every 0.5 seconds, not a large game and the physics cone radar scan method was being difficult so for now like this, might change in future
	DetectedTargets.Empty();
	TArray<AActor*> AllAircraft;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APawn::StaticClass(), AllAircraft);

	for (AActor* Target : AllAircraft) 
	{
		APawn* RegisteredPawn = Cast<APawn>(Target);
		if (RegisteredPawn && RegisteredPawn != this) 
		{
			FDetectedAircraftInfo TempInfo;
			TempInfo.Location = RegisteredPawn->GetActorLocation();
			TempInfo.Distance = FVector::Dist(this->GetActorLocation(), RegisteredPawn->GetActorLocation());
			TempInfo.Rotation = RegisteredPawn->GetActorRotation();
			TempInfo.threatLevel = TempInfo.CalculateThreat();

			if (TempInfo.threatLevel > 0) 
			{
				TempInfo.CurrentPawn = RegisteredPawn;
				DetectedTargets.Add(TempInfo);
			}
		}
	}

	if (!Targeting.CurrentPawn || (Targeting.CurrentPawn && Targeting.threatLevel < 0.5)) 
	{
		PickTarget();
	}
}

//Currently targeting only the User, once more actors are placed I will figure out some math formula to calculate a system to target enemies
void AF16AI::PickTarget() 
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
		else {
			Targeting = DetectedTargets[i];
		}
	}
}

void AF16AI::RollToTarget(float RollInput, float DeltaSeconds) 
{
	if (GetWorld()) 
	{
		//Application of Roll
		currentRollAI = FMath::FInterpTo(currentRollAI, RollInput, DeltaSeconds, 8.f);

		//Roll in DeltaSeconds Terms
		float DeltaRoll = -(currentRollAI * RollRate)* DeltaSeconds;
		DeltaRoll = FMath::Clamp(DeltaRoll, -RollRate * DeltaSeconds, RollRate * DeltaSeconds);

		FQuat DeltaRot = FQuat(FRotator(0, 0, DeltaRoll));

		this->AddActorLocalRotation(DeltaRot);
	}
}

FDetectedAircraftInfo AF16AI::ReturnTargeting() 
{
	return Targeting;
}

//Animation (Not yet)

/*

	void PitchElevatorCalculation(float DeltaSeconds);

	void RollElevatorCalculation(float DeltaSeconds);

	void SlatsElevationCalculation(float DeltaSeconds);

	void RudderYawCalculation(float DeltaSeconds);

	void FlapPitchCalculation(float DeltaSeconds);
*/