// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("CONTROL!"));
#include "Gamemodes/FlightGamemode.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/PlayerStart.h"
#include "AircraftPlayerController.h"
#include "Gamemodes/CurrentPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Aircraft/AI/F16AI.h"
#include "Gamemodes/PlayerGameInstance.h"
#include "UI/PlayerHUD.h"
#include "Aircraft/AI/EnemyAircraftAI.h"
#include "EngineUtils.h"
#include "AircraftRegistry.h"
#include "Aircraft/WeaponSystemComponent.h"
#include "Weapons/BaseWeapon.h"

AFlightGamemode::AFlightGamemode() 
{
}

void AFlightGamemode::BeginPlay() 
{
	if (!GetWorld()) return;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (!AAircraftRegistry::Get(GetWorld())) 
	{
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

	//SpawnAIAircraft();
	Super::BeginPlay();
}

// TODO: Move this logic into actual AI spawn points and just move them around in the editor

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

void AFlightGamemode::HandlePlayerState(AAircraftPlayerController* PlayerControl) 
{
	APlayerStart* PlayerStart = nullptr;

	UPlayerGameInstance* GI = GetWorld()->GetGameInstance<UPlayerGameInstance>();

	if (!GI) return;

	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It) 
	{
		PlayerStart = *It;
		break;
	}

	if (!PlayerStart || !HasAuthority()) return;

	if (!GI->SelectedAircraft) 
	{
		FallBackAircraft();
	}
	else 
	{
		PlayerSpawnedIn = GetWorld()->SpawnActor<APlayerAircraft>(GI->SelectedAircraft->AircraftClass, PlayerStart->GetActorTransform());
	}

	TMap<FName, TSubclassOf<ABaseWeapon>> Loadout;

	Loadout = GI->SelectedWeapons;

	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
		{
			PlayerSpawnedIn->SetOwner(PC);
			PC->Possess(PlayerSpawnedIn);
			PlayerSpawnedIn->PossessedBy(PC);
		});

	PlayerSpawnedIn->WeaponComponent->SetWeapons(Loadout);
}

// TODO: Might remove later, might keep in for default, unsure for now, maybe leave it in for a fun easter egg

void AFlightGamemode::FallBackAircraft() 
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

	if (!AircraftSelected || !HasAuthority() || !AircraftSelected->AircraftClass) return;

	APlayerStart* PlayerStart = nullptr;

	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		PlayerStart = *It;
		break;
	}

	if (!PlayerStart) return;
	PlayerSpawnedIn = GetWorld()->SpawnActor<APlayerAircraft>(AircraftSelected->AircraftClass, PlayerStart->GetActorTransform());
}

// TODO: Remove this later, this is only to set different weapons for testing

TMap<FName, TSubclassOf<ABaseWeapon>> AFlightGamemode::TemporaryLoadout()
{
	TMap<FName, TSubclassOf<ABaseWeapon>> Loadout;

	if (!AircraftSelected->AircraftStat) return Loadout;

	if (AircraftSelected->AircraftStat->NumOfPylons < 2) return Loadout;

	for (int i = 0; i < 2; i++) 
	{
		FString PylonName = FString::Printf(TEXT("Pylon%d"), i);
		FName Pylon(*PylonName);
		Loadout.Add(Pylon, Bomb);
	}

	for (int i = 2; i < AircraftSelected->AircraftStat->NumOfPylons; i++) 
	{
		FString PylonName = FString::Printf(TEXT("Pylon%d"), i);
		FName Pylon(*PylonName);
		Loadout.Add(Pylon, Missile);
	}

	return Loadout;
}