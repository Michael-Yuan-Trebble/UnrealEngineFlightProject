// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Gamemode!"));
#include "Gamemodes/AircraftSelectionGamemode.h"
#include "GameFramework/SpectatorPawn.h"
#include "UI/SelectionUI/AircraftSelectionWidget.h"
#include "Player Info/AircraftPlayerController.h"
#include "Units/Aircraft/Player/PlayerAircraft.h"
#include "Units/Aircraft/MenuManagerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Enums/MenuState.h"
#include "Player Info/PlayerGameInstance.h"
#include "Subsystem/LevelTransitionSubsystem.h"
#include "Subsystem/MissionManagerSubsystem.h"
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
		if (!IsValid(PC) || !IsValid(PC->GetMenuManager())) return;
		PC->GetMenuManager()->SetupClasses(AircraftSelectClass, WeaponSelectClass, BuySelectionClass, SpecialSelectionClass);
		PC->GetMenuManager()->ChooseAircraftUI();
	});
}

void AAircraftSelectionGamemode::SpawnInAircraft(const TSubclassOf<APawn> SpawnIn) 
{
	if (AircraftDisplayed && AircraftDisplayed->GetClass() == SpawnIn) return;

	if (AircraftDisplayed) 
	{
		AircraftDisplayed->Destroy();
		AircraftDisplayed = nullptr;
	}

	FVector PreviewLocation = FVector::ZeroVector;

	FHitResult HitResult;

	FCollisionQueryParams Params;
	Params.bTraceComplex = true;

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		PreviewLocation + FVector(0, 0, 10000),
		PreviewLocation - FVector(0, 0, 10000),
		ECC_WorldStatic,
		Params
	);

	float SpawnOffset = 150.f;

	FVector SpawnLocation = bHit ? HitResult.Location + FVector(0, 0, SpawnOffset) : PreviewLocation;

	FRotator PreviewRotation = FRotator::ZeroRotator;
	AircraftDisplayed = GetWorld()->SpawnActor<APawn>(SpawnIn, SpawnLocation, PreviewRotation, SpawnParams);
	ABaseAircraft* Preview = Cast<ABaseAircraft>(AircraftDisplayed);
	Preview->SetLandingGearVisiblility(true);
	
}

void AAircraftSelectionGamemode::SpawnInWeapon(const TSubclassOf<ABaseWeapon> Weapon, const FName& Pylon) 
{
	if (!AircraftDisplayed || !Weapon) return;

	ABaseAircraft* BaseAircraft = Cast<ABaseAircraft>(AircraftDisplayed);
	if (!BaseAircraft) return;

	USkeletalMeshComponent* Mesh = BaseAircraft->GetAirframe();
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

void AAircraftSelectionGamemode::ClearWeapons(const FName& Pylon) 
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

	TSoftObjectPtr<UWorld> LoadWorld = GI->GetLevel().Level;

	// Default to TestHeightmap, might change this later?
	if (LoadWorld.IsNull())
	{
		// TODO: Do some fallback, for now return
		return;
	}

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC) return;

	AAircraftPlayerController* LocalAPC = IsValid(APC) ? APC.Get() : Cast<AAircraftPlayerController>(PC);

	if (IsValid(LocalAPC) && IsValid(LocalAPC->GetMenuManager())) 
	{ 
		LocalAPC->DisableInput(Cast<APlayerController>(PC));
		if (IsValid(LocalAPC->GetMenuManager())) LocalAPC->GetMenuManager()->CloseAll();
	}

	ABaseAircraft* BaseAir = Cast<ABaseAircraft>(AircraftDisplayed);
	
	// TODO: See what would work better, returning if there is no aircraft displayed or letting it ride and just using the fallback

	if (BaseAir) 
	{
		FullLoadout.AircraftClass = BaseAir->GetClass();
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

	FullLoadout.EquippedWeapons = Loadout;
	GI->SetLoadout(FullLoadout);

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
	if (IsValid(APC->GetMenuManager())) World->GetTimerManager().ClearAllTimersForObject(APC->GetMenuManager());

	UMissionManagerSubsystem* MissionManager = GI->GetSubsystem<UMissionManagerSubsystem>();
	ULevelTransitionSubsystem* LevelTransition = GI->GetSubsystem<ULevelTransitionSubsystem>();

	const FMissionData& Mission = MissionManager->GetCurrentMission();
	LevelTransition->LoadIntermission(Mission.TakeoffType);
}