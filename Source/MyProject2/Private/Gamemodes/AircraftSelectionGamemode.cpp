// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Gamemode!"));
#include "Gamemodes/AircraftSelectionGamemode.h"
#include "GameFramework/SpectatorPawn.h"
#include "UI/SelectionUI/AircraftSelectionWidget.h"
#include "Player Info/AircraftPlayerController.h"
#include "Units/Aircraft/Player/PlayerAircraft.h"
#include "Units/Aircraft/MenuManagerComponent.h"
#include "Enums/ControlModeTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Enums/MenuState.h"
#include "Player Info/PlayerGameInstance.h"
#include "Units/Aircraft/BaseAircraft.h"

FActorSpawnParameters SpawnParams;

AAircraftSelectionGamemode::AAircraftSelectionGamemode() 
{
}

void AAircraftSelectionGamemode::BeginPlay() 
{
	Super::BeginPlay();

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC)  return;

	APC = Cast<AAircraftPlayerController>(PC);
	if (!APC)

	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ASpectatorPawn* Spectator = GetWorld()->SpawnActor<ASpectatorPawn>(ASpectatorPawn::StaticClass(), FVector::ZeroVector,FRotator::ZeroRotator,SpawnParams);

	if (!Spectator)  return;

	APC->Possess(Spectator);
	APC->SetControlMode(EControlMode::Menu);
	TWeakObjectPtr<AAircraftPlayerController> WeakAPC = APC;

	GetWorld()->GetTimerManager().SetTimerForNextTick([this, WeakAPC]() 
	{
		if (!WeakAPC.IsValid()) return;
		AAircraftPlayerController* PC = WeakAPC.Get();
		if (!IsValid(PC) || !IsValid(PC->MenuManager)) return;
		PC->MenuManager->SetupClasses(AircraftSelectClass, WeaponSelectClass, BuySelectionClass, SpecialSelectionClass);
		PC->MenuManager->ChooseAircraftUI();
	});
}

void AAircraftSelectionGamemode::SpawnInAircraft(TSubclassOf<APawn> SpawnIn) 
{
	if (AircraftDisplayed && AircraftDisplayed->GetClass() == SpawnIn) return;

	if (AircraftDisplayed) 
	{
		AircraftDisplayed->Destroy();
		AircraftDisplayed = nullptr;
	}

	FVector PreviewLocation = FVector::ZeroVector;
	FRotator PreviewRotation = FRotator::ZeroRotator;
	AircraftDisplayed = GetWorld()->SpawnActor<APawn>(SpawnIn, PreviewLocation, PreviewRotation, SpawnParams);
}

void AAircraftSelectionGamemode::SpawnInWeapon(TSubclassOf<ABaseWeapon> Weapon, FName Pylon) 
{
	if (!AircraftDisplayed || !Weapon) return;

	ABaseAircraft* BaseAircraft = Cast<ABaseAircraft>(AircraftDisplayed);
	if (!BaseAircraft) return;

	USkeletalMeshComponent* Mesh = BaseAircraft->Airframe;
	if (!IsValid(Mesh) || !Mesh->DoesSocketExist(Pylon)) return;

	AActor* WeaponDisplayed;
	FTransform SocketTransform = Mesh->GetSocketTransform(Pylon);
	if (AActor** WeaponPtr = EquippedWeapons.Find(Pylon)) 
	{
		WeaponDisplayed = *WeaponPtr;
		if (IsValid(WeaponDisplayed))
		{
			WeaponDisplayed->Destroy();
		}
	}

	WeaponDisplayed = GetWorld()->SpawnActor<AActor>(Weapon, SocketTransform, SpawnParams);
	if (!IsValid(WeaponDisplayed)) return;
	WeaponDisplayed->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, Pylon);
	EquippedWeapons.Add(Pylon, WeaponDisplayed);
}

void AAircraftSelectionGamemode::ClearWeapons(FName Pylon) 
{
	if (!IsValid(AircraftDisplayed)) return;

	AActor** WeaponPtr = EquippedWeapons.Find(Pylon);
	if (!WeaponPtr) return;

	AActor* WeaponDisplayed = *WeaponPtr;
	if (IsValid(WeaponDisplayed)) 
	{
		WeaponDisplayed->Destroy();
	}

	EquippedWeapons.Remove(Pylon);
}

void AAircraftSelectionGamemode::EndSelection(AAircraftPlayerController* Controller)
{
	if (!ReadyPlayers.Contains(Controller)) 
	{
		ReadyPlayers.Add(Controller);
		TryAdvanceToNextStage();
	}
}

// TODO: Try to make it so it advances to a "Buffer" screen
void AAircraftSelectionGamemode::TryAdvanceToNextStage() 
{
	// TODO: Make Co-Op be able to work if implemented
	//if (ReadyPlayers.Num() < PlayersRequired) return;

	UWorld* World = GetWorld();
	if (!World) return;

	UPlayerGameInstance* GI = GetWorld()->GetGameInstance<UPlayerGameInstance>();
	if (!GI) return;

	FString LevelNameString = GI->LevelName.ToString();

	// Default to TestHeightmap, might change this later?
	if (GI->LevelName.IsNone() || LevelNameString.IsEmpty())
	{
		LevelNameString = TEXT("TestHeightmap");
	}

	FString MapPath = FString::Printf(TEXT("/Game/Maps/%s"), *LevelNameString);

	if (!FPackageName::DoesPackageExist(MapPath)) return;

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC) return;

	AAircraftPlayerController* LocalAPC = IsValid(APC) ? APC : Cast<AAircraftPlayerController>(PC);

	if (IsValid(LocalAPC) && IsValid(LocalAPC->MenuManager)) 
	{ 
		LocalAPC->DisableInput(Cast<APlayerController>(PC));
		if (IsValid(LocalAPC->MenuManager)) LocalAPC->MenuManager->CloseAll();
	}

	ABaseAircraft* BaseAir = Cast<ABaseAircraft>(AircraftDisplayed);
	
	// TODO: See what would work better, returning if there is no aircraft displayed or letting it ride and just using the fallback

	if (BaseAir) 
	{
		GI->SetClass(BaseAir->GetClass());

		// TODO: Maybe get rid of this as once aircraft spawns no need for a dedicated airstats in GI
		GI->SelectedAircraftStats = BaseAir->AirStats;
	}

	// TODO: Change funtions so that they suit this

	TMap<FName, TSubclassOf<ABaseWeapon>> Loadout;

	for (auto& Pair : EquippedWeapons)
	{
		AActor* Actor = Pair.Value;
		if (!IsValid(Actor)) continue;

		ABaseWeapon* Weapon = Cast<ABaseWeapon>(Actor);
		if (!Weapon) continue;

		Loadout.Add(Pair.Key, Weapon->GetClass());
	}

	GI->SetWeapons(Loadout);

	// TODO: Set Specials HERE

	for (auto& Pair : EquippedWeapons)
	{
		if (Pair.Value && Pair.Value->IsValidLowLevel())
		{
			Pair.Value->Destroy();
		}
	}
	EquippedWeapons.Empty();

	if (IsValid(AircraftDisplayed))
	{
		AircraftDisplayed->Destroy();
		AircraftDisplayed = nullptr;
	}

	World->GetTimerManager().ClearAllTimersForObject(this);
	World->GetTimerManager().ClearAllTimersForObject(APC);
	if (IsValid(APC->MenuManager)) World->GetTimerManager().ClearAllTimersForObject(APC->MenuManager);

	FName LevelToOpen(*LevelNameString);
	UGameplayStatics::OpenLevel(World, LevelToOpen);
}