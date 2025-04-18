// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("CONTROL!"));
#include "FlightGamemode.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/PlayerController.h"
#include "AircraftPlayerController.h"
#include "CurrentPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "R60.h"
#include "F16AI.h"
#include "EnemyAircraftAI.h"
#include "Su25Pawn.h"

AFlightGamemode::AFlightGamemode() {

	DefaultPawnClass = nullptr;
	PlayerControllerClass = AAircraftPlayerController::StaticClass();

	//Static currently, Spawn in a specific Blueprint for User
	static ConstructorHelpers::FClassFinder<ASu25Pawn> Su25PawnBPClass(TEXT("/Game/Aircraft/Su25/Su25Pawn"));
	if (Su25PawnBPClass.Succeeded())
	{
		Player = Su25PawnBPClass.Class;
	}

	//Static currently, Spawn in a specific Blueprint for AI
	static ConstructorHelpers::FClassFinder<AF16AI> F16AIBPClass(TEXT("/Game/AI/F16Enemy"));
	if (F16AIBPClass.Succeeded()) 
	{
		AIAircraftClass = F16AIBPClass.Class;
	}
}

void AFlightGamemode::BeginPlay() 
{
	Super::BeginPlay();

	//Spawn in Player
	if (Player) 
	{
		FVector SpawnLocation = FVector(0, 0, 0);
		FRotator SpawnRotation = FRotator(0, 0, 0);
		ASu25Pawn* Spawned = GetWorld()->SpawnActor<ASu25Pawn>(Player, SpawnLocation, SpawnRotation);
		if (Spawned) 
		{
			GetWorld()->GetTimerManager().SetTimerForNextTick([this, Spawned]()
				{
					AAircraftPlayerController* PlayerController = Cast<AAircraftPlayerController>(GetWorld()->GetFirstPlayerController());
					if (PlayerController) 
					{
						//Possession currently is being a bit buggy, might be the point of concern
						Spawned->SetOwner(PlayerController);
						PlayerController->Possess(Spawned);
						Spawned->PossessedBy(PlayerController);
						//Player State for later
						HandlePlayerState(PlayerController, Spawned);
					}
				});
		}
	}
	//Spawns in a statically chosen AI
	SpawnAIAircraft();
}

void AFlightGamemode::SpawnAIAircraft() 
{
	FVector SpawnLocation = FVector(0, 0, 0);
	FRotator SpawnRotation = FRotator(0, 0, 0);
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AF16AI* AIAir = GetWorld()->SpawnActor<AF16AI>(AIAircraftClass, SpawnLocation, SpawnRotation, SpawnParameters);
	if (AIAir) 
	{
		AEnemyAircraftAI* AIControl = GetWorld()->SpawnActor<AEnemyAircraftAI>(AEnemyAircraftAI::StaticClass());
		if (AIControl) 
		{
			AIControl->Possess(AIAir);
		}
	}
}

void AFlightGamemode::HandlePlayerState(AAircraftPlayerController* PlayerControl, ABaseAircraft* ControlledAir) 
{
	APlayerState* PS = PlayerControl->PlayerState;
	if (PS) 
	{
		//Currently Static Arming of plane, testing out stuff
		TArray<TSubclassOf<ABaseIRMissile>> Loadout;
		Loadout.Add(AR60::StaticClass());
		Loadout.Add(AR60::StaticClass());
		ControlledAir->EquipWeapons(Loadout);
	}
}