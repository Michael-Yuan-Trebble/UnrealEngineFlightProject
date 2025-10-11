// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("CONTROL!"));
#include "Gamemodes/FlightGamemode.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/PlayerStart.h"
#include "AircraftPlayerController.h"
#include "Gamemodes/CurrentPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Aircraft/AI/F16AI.h"
#include "Aircraft/Player/Su27Pawn.h"
#include "UI/PlayerHUD.h"
#include "Aircraft/AI/EnemyAircraftAI.h"
#include "EngineUtils.h"
#include "AircraftRegistry.h"
#include "Aircraft/WeaponSystemComponent.h"
#include "Weapons/BaseWeapon.h"
#include "Aircraft/Player/T38Pawn.h"

AFlightGamemode::AFlightGamemode() 
{
	// ====================================
	// Temporary Fill In classes for testing
	// ====================================
	static ConstructorHelpers::FClassFinder<ASu27Pawn> T38PawnBPClass(TEXT("/Game/Aircraft/Su27/Su27BP.Su27BP"));
	if (T38PawnBPClass.Succeeded())
	{
		Player = T38PawnBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<AF16AI> F16AIBPClass(TEXT("/Game/AI/F16DefaultAI"));
	if (F16AIBPClass.Succeeded()) 
	{
		AIAircraftClass = F16AIBPClass.Class;
	}
	static ConstructorHelpers::FClassFinder<ABaseWeapon> Aim9BPClass(TEXT("/Game/Weapons/R-77/R-77BP"));
	if (Aim9BPClass.Succeeded()) 
	{
		Aim9 = Aim9BPClass.Class;
	}
}

void AFlightGamemode::BeginPlay() 
{
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (!AAircraftRegistry::Get(GetWorld())) {
		GetWorld()->SpawnActor<AAircraftRegistry>(AAircraftRegistry::StaticClass(), FVector::ZeroVector,FRotator::ZeroRotator,Params);
	}

	PC = Cast<AAircraftPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (!PC) return;

	PS = Cast<ACurrentPlayerState>(PC->PlayerState);
	if (!PS) return;

	Database = NewObject<UAircraftDatabase>(PC->GetGameInstance());
	if (!Database) return;

	FString Path = "/Game/Aircraft/AircraftData";
	Database->LoadAllAircraftFromFolder(Path);

	HandlePlayerState(PC);

	if (PlayerSpawnedIn) 
	{ 
		PC->SetControlMode(EControlMode::Aircraft);
	}

	Super::BeginPlay();
	//SpawnAIAircraft();
}

void AFlightGamemode::SpawnAIAircraft() 
{
	APlayerStart* PlayerStart = nullptr;
	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It) 
	{
		PlayerStart = *It;
		break;
	}
	if (PlayerStart) 
	{
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

// TODO: Make Playerstate actually set the aircraft and other necessities

void AFlightGamemode::HandlePlayerState(AAircraftPlayerController* PlayerControl) 
{
	if (Database->AllAircraft.Num() == 0) return;
	for (UAircraftData* Data : Database->AllAircraft)
	{
		if (Data && Data->AircraftClass == Player)
		{
			AircraftSelected = Data;
			break;
		}
	}
	//if (!HasAuthority()) return;
	if (!AircraftSelected) return;

	if (!AircraftSelected->AircraftClass) return;
	APlayerStart* PlayerStart = nullptr;

	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It) 
	{
		PlayerStart = *It;
		break;
	}

	if (!PlayerStart) return;
	PlayerSpawnedIn = GetWorld()->SpawnActor<APlayerAircraft>(AircraftSelected->AircraftClass, PlayerStart->GetActorTransform());

	ABaseAircraft* SpawnedIn = PlayerSpawnedIn;
	TMap<FName, TSubclassOf<ABaseWeapon>> Loadout;
	for (int i = 0; i < AircraftSelected->AircraftStat->NumOfPylons; i++) {
		FString PylonName = FString::Printf(TEXT("Pylon%d"), i);
		FName Pylon(*PylonName);
		Loadout.Add(Pylon, Aim9);
	}

	GetWorld()->GetTimerManager().SetTimerForNextTick([this, SpawnedIn]()
		{
			SpawnedIn->SetOwner(PC);
			PC->Possess(SpawnedIn);
			SpawnedIn->PossessedBy(PC);
		});

	PlayerSpawnedIn->WeaponComponent->SetWeapons(Loadout);
}