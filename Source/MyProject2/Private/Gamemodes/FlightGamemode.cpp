// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("CONTROL!"));
#include "Gamemodes/FlightGamemode.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "AircraftPlayerController.h"
#include "Gamemodes/CurrentPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Weapons/Missiles/R60.h"
#include "Aircraft/AI/F16AI.h"
#include "UI/PlayerHUD.h"
#include "GameFramework/SpectatorPawn.h"
#include "Aircraft/AI/EnemyAircraftAI.h"
#include "Aircraft/Player/Su25Pawn.h"
#include "EngineUtils.h"
#include "Aircraft/Player/T38Pawn.h"

AFlightGamemode::AFlightGamemode() 
{
	DefaultPawnClass = nullptr;
	PlayerControllerClass = AAircraftPlayerController::StaticClass();
	HUDClass = APlayerHUD::StaticClass();

	//Static currently, Spawn in a specific Blueprint for User
	static ConstructorHelpers::FClassFinder<ASu25Pawn> Su25PawnBPClass(TEXT("/Game/Aircraft/Su25/Su25Pawn"));
	if (Su25PawnBPClass.Succeeded())
	{
		//Player = Su25PawnBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<AT38Pawn> T38PawnBPClass(TEXT("/Game/Aircraft/T-38/T-38Pawn"));
	if (T38PawnBPClass.Succeeded()) {
		Player = T38PawnBPClass.Class;
	}

	//Static currently, Spawn in a specific Blueprint for AI
	static ConstructorHelpers::FClassFinder<AF16AI> F16AIBPClass(TEXT("/Game/AI/F16DefaultAI"));
	if (F16AIBPClass.Succeeded()) 
	{
		AIAircraftClass = F16AIBPClass.Class;
	}
}

void AFlightGamemode::BeginPlay() 
{
	Super::BeginPlay();

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC) 
	{
		if (AAircraftPlayerController* APC = Cast<AAircraftPlayerController>(PC))
		{
			APC->SetControlMode(EControlMode::Aircraft);
		}
	}

	//Spawn in Player
	if (Player) 
	{
		APlayerStart* PlayerStart = nullptr;
		for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It) {
			PlayerStart = *It;
			break;
		}
		if (PlayerStart) {
			FVector SpawnLocation = PlayerStart->GetActorLocation();
			FRotator SpawnRotation = PlayerStart->GetActorRotation();
			//ASu25Pawn* Spawned = GetWorld()->SpawnActor<ASu25Pawn>(Player, SpawnLocation, SpawnRotation);
			AT38Pawn* Spawned = GetWorld()->SpawnActor<AT38Pawn>(Player, SpawnLocation, SpawnRotation);
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
		}
	//Spawns in a statically chosen AI
	SpawnAIAircraft();
}

void AFlightGamemode::SpawnAIAircraft() 
{
	APlayerStart* PlayerStart = nullptr;
	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It) {
		PlayerStart = *It;
		break;
	}
	if (PlayerStart) {
		FVector OffSet(10, 10, 0);
		FVector SpawnLocation = PlayerStart->GetActorLocation() + OffSet;
		FRotator SpawnRotation = PlayerStart->GetActorRotation();
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
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
}

void AFlightGamemode::HandlePlayerState(AAircraftPlayerController* PlayerControl, ABaseAircraft* ControlledAir) 
{
	APlayerState* PS = PlayerControl->PlayerState;
	if (PS) 
	{
		//Currently Static Arming of plane, testing out stuff
		//TArray<TSubclassOf<ABaseIRMissile>> Loadout;
		//Loadout.Add(AR60::StaticClass());
		//Loadout.Add(AR60::StaticClass());
		//ControlledAir->EquipWeapons(Loadout);
	}
}