// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("CONTROL!"));
#include "Gamemodes/FlightGamemode.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/PlayerStart.h"
#include "AircraftPlayerController.h"
#include "Gamemodes/CurrentPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Aircraft/Player/PlayerAircraft.h"
#include "Gamemodes/PlayerGameInstance.h"
#include "UI/PlayerHUD.h"
#include "EngineUtils.h"
#include "AircraftRegistry.h"
#include "Weapons/BaseWeapon.h"

AFlightGamemode::AFlightGamemode() 
{
}

void AFlightGamemode::BeginPlay() 
{
	Super::BeginPlay();

	if (!GetWorld()) return;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

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
		SetPlayerSpeed();
	}
}

// TODO: Move this logic into actual AI spawn points and just move them around in the editor

void AFlightGamemode::SpawnAIAircraft() 
{
}

void AFlightGamemode::SetPlayerSpeed() {
	if (!PlayerSpawnedIn) return;
	PlayerSpawnedIn->SetSpeed(PlayerSpawnSpeed/0.034);
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

	if (!GI->AircraftClass)
	{
		FallBackAircraft();
	}
	else
	{
		PlayerSpawnedIn = GetWorld()->SpawnActorDeferred<APlayerAircraft>(GI->AircraftClass, PlayerStart->GetActorTransform());
	}

	TMap<FName, TSubclassOf<ABaseWeapon>> Loadout;
	
	// TODO: Change later, its fine if the weapon is empty the user can choose that, just easier for testing for now

	if (GI->SelectedWeapons.IsEmpty()) 
	{
		Loadout = TemporaryLoadout();
	}
	else 
	{
		Loadout = GI->SelectedWeapons;
	}

	if (!PlayerSpawnedIn) return;

	UGameplayStatics::FinishSpawningActor(PlayerSpawnedIn, PlayerStart->GetActorTransform());

	PlayerSpawnedIn->SetWeapons(Loadout);

	if (IsValid(PC))
	{
		PC->Possess(PlayerSpawnedIn);
	}
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
	PlayerSpawnedIn = GetWorld()->SpawnActorDeferred<APlayerAircraft>(AircraftSelected->AircraftClass, PlayerStart->GetActorTransform());
}

// TODO: Remove this later, this is only to set different weapons for testing

TMap<FName, TSubclassOf<ABaseWeapon>> AFlightGamemode::TemporaryLoadout()
{
	TMap<FName, TSubclassOf<ABaseWeapon>> Loadout;

	if (!AircraftSelected || !AircraftSelected->AircraftStat) return Loadout;

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