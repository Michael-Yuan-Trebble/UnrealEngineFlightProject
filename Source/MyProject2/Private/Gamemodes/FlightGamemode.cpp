// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("CONTROL!"));
#include "Gamemodes/FlightGamemode.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/PlayerStart.h"
#include "Player Info/AircraftPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Units/Aircraft/Player/PlayerAircraft.h"
#include "Player Info/PlayerGameInstance.h"
#include "UI/PlayerHUD.h"
#include "EngineUtils.h"
#include "Subsystem/AircraftRegistry.h"
#include "Weapons/BaseWeapon.h"

AFlightGamemode::AFlightGamemode() 
{
}

void AFlightGamemode::BeginPlay() 
{
	Super::BeginPlay();

	SpawnInController();
}

void AFlightGamemode::SpawnInController() 
{
	if (!GetWorld()) return;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	PC = Cast<AAircraftPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (!IsValid(PC)) return;

	Database = NewObject<UAircraftDatabase>(PC->GetGameInstance());
	if (!IsValid(Database)) return;

	FString Path = "/Game/Aircraft/AircraftData";
	Database->LoadAllAircraftFromFolder(Path);

	HandlePlayerState(PC);

	if (IsValid(PlayerSpawnedIn))
	{
		PC->SetControlMode(EControlMode::Aircraft);
		SetPlayerSpeed();
	}
}


// Tells flight comp which mode to be set on
void AFlightGamemode::SetFlightMode() 
{
	if (IsValid(PlayerSpawnedIn)) PlayerSpawnedIn->SetFlightMode(FlightMode);
}

// TODO: Move this logic into actual AI spawn points and just move them around in the editor

void AFlightGamemode::SpawnAIAircraft() 
{
}

void AFlightGamemode::SetPlayerSpeed() 
{
	if (!IsValid(PlayerSpawnedIn)) return;
	PlayerSpawnedIn->SetSpeed(PlayerSpawnSpeed/0.034);
}

void AFlightGamemode::HandlePlayerState(AAircraftPlayerController* PlayerControl)
{
	APlayerStart* PlayerStart = nullptr;

	UPlayerGameInstance* GI = GetWorld()->GetGameInstance<UPlayerGameInstance>();

	if (!IsValid(GI)) return;

	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		PlayerStart = *It;
		break;
	}

	if (!IsValid(PlayerStart) || !HasAuthority()) return;

	if (!IsValid(GI->AircraftClass))
	{
		FallBackAircraft();
	}
	else
	{
		FTransform SpawnTransform = PlayerStart->GetActorTransform();
		PlayerSpawnedIn = GetWorld()->SpawnActorDeferred<APlayerAircraft>(GI->AircraftClass, SpawnTransform);
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

	if (!IsValid(PlayerSpawnedIn)) return;

	UGameplayStatics::FinishSpawningActor(PlayerSpawnedIn, PlayerStart->GetActorTransform());

	PlayerSpawnedIn->SetWeapons(Loadout);

	if (IsValid(Special)) PlayerSpawnedIn->SetSpecial(Special);

	if (IsValid(PC))
	{
		PC->Possess(PlayerSpawnedIn);
	}
	SetFlightMode();
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

	if (!IsValid(AircraftSelected) || !HasAuthority() || !IsValid(AircraftSelected->AircraftClass)) return;

	APlayerStart* PlayerStart = nullptr;

	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		PlayerStart = *It;
		break;
	}

	if (!IsValid(PlayerStart)) return;
	FTransform Transform = PlayerStart->GetActorTransform();
	PlayerSpawnedIn = GetWorld()->SpawnActorDeferred<APlayerAircraft>(AircraftSelected->AircraftClass, Transform);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Z: %.2f"), Transform.GetLocation().Z));
}

// TODO: Remove this later, this is only to set different weapons for testing

TMap<FName, TSubclassOf<ABaseWeapon>> AFlightGamemode::TemporaryLoadout()
{
	TMap<FName, TSubclassOf<ABaseWeapon>> Loadout;

	if (!IsValid(AircraftSelected) || !AircraftSelected->AircraftStat) return Loadout;

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