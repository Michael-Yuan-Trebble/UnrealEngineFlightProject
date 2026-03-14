// Fill out your copyright notice in the Description page of Project Settings.

#include "Gamemodes/AircraftSelectionGamemode.h"
#include "GameFramework/SpectatorPawn.h"
#include "Player Info/AircraftPlayerController.h"
#include "Units/Components/Player/MenuManagerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player Info/PlayerGameInstance.h"
#include "Subsystem/LevelTransitionSubsystem.h"
#include "Subsystem/MissionManagerSubsystem.h"
#include "Units/Aircraft/BaseAircraft.h"
#include "Debug/DebugHelper.h"

FActorSpawnParameters SpawnParams;

AAircraftSelectionGamemode::AAircraftSelectionGamemode() 
{
}

void AAircraftSelectionGamemode::BeginPlay() 
{
	Super::BeginPlay();
	
	if (!IsValid(GetWorld())) return;

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0)) {
		APC = Cast<AAircraftPlayerController>(PC);
	}

	if (!IsValid(APC)) return;

	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ASpectatorPawn* Spectator = GetWorld()->SpawnActor<ASpectatorPawn>(ASpectatorPawn::StaticClass(), FVector::ZeroVector,FRotator::ZeroRotator,SpawnParams);
	if (!IsValid(Spectator)) return;

	APC->Possess(Spectator);
	APC->SetControlMode(EControlMode::Menu);
	TWeakObjectPtr<AAircraftPlayerController> WeakAPC = APC;

	GetWorld()->GetTimerManager().SetTimerForNextTick([this, WeakAPC]() 
	{
		if (!WeakAPC.IsValid()) return;
		AAircraftPlayerController* PC = WeakAPC.Get();
		if (!IsValid(PC) || !IsValid(PC->GetMenuManager())) return;
		PC->GetMenuManager()->SetupClasses(AircraftSelectClass, WeaponSelectClass, BuySelectionClass, SpecialSelectionClass, GreyOutClass);
		PC->GetMenuManager()->ChooseAircraftUI();
	});
}

void AAircraftSelectionGamemode::SpawnInAircraft(const TSubclassOf<APawn> SpawnIn) 
{
	if (!IsValid(GetWorld())) return;

	if (IsValid(AircraftDisplayed)) {
		if (AircraftDisplayed->GetClass() == SpawnIn->GetClass()) return;

		AircraftDisplayed->Destroy();
		AircraftDisplayed = nullptr;
	}

	FVector PreviewLocation = FVector::ZeroVector;

	FHitResult HitResult{};

	FCollisionQueryParams Params;
	Params.bTraceComplex = true;

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		PreviewLocation,
		PreviewLocation - FVector(0, 0, 10000),
		ECC_WorldStatic,
		Params
	);

	float SpawnOffset = 250.f;

	FVector SpawnLocation = bHit ? HitResult.Location + FVector(0, 0, SpawnOffset) : PreviewLocation;
	FRotator PreviewRotation = FRotator::ZeroRotator;
	AircraftDisplayed = GetWorld()->SpawnActor<APawn>(SpawnIn, SpawnLocation, PreviewRotation, SpawnParams);
	if (!IsValid(AircraftDisplayed)) return;
	if (ABaseAircraft* Preview = Cast<ABaseAircraft>(AircraftDisplayed)) {
		Preview->SetLandingGearVisiblility(true);
	}
	
}

void AAircraftSelectionGamemode::SpawnInWeapon(const TSubclassOf<ABaseWeapon> Weapon, const FName& Pylon) 
{
	if (!IsValid(AircraftDisplayed) || !IsValid(Weapon)) return;

	ABaseAircraft* BaseAircraft = Cast<ABaseAircraft>(AircraftDisplayed);
	if (!IsValid(BaseAircraft)) return;

	USkeletalMeshComponent* Mesh = BaseAircraft->GetAirframe();
	if (!IsValid(Mesh) || !Mesh->DoesSocketExist(Pylon)) return;

	AActor* WeaponDisplayed;
	FTransform SocketTransform = Mesh->GetSocketTransform(Pylon);
	if (TObjectPtr<AActor>* WeaponPtr = EquippedWeapons.Find(Pylon))
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

	TObjectPtr<AActor>* WeaponPtr = EquippedWeapons.Find(Pylon);
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
	if (!IsValid(World)) return;

	UPlayerGameInstance* GI = World->GetGameInstance<UPlayerGameInstance>();
	APlayerController* PC = World->GetFirstPlayerController();
	if (!IsValid(GI) || !IsValid(PC)) return;

	AAircraftPlayerController* LocalAPC = IsValid(APC) ? APC.Get() : Cast<AAircraftPlayerController>(PC);

	if (IsValid(LocalAPC) && IsValid(LocalAPC->GetMenuManager())) 
	{ 
		LocalAPC->DisableInput(Cast<APlayerController>(PC));
		if (IsValid(LocalAPC->GetMenuManager())) LocalAPC->GetMenuManager()->CloseAll();
	}
	
	// TODO: See what would work better, returning if there is no aircraft displayed or letting it ride and just using the fallback

	if (ABaseAircraft* BaseAir = Cast<ABaseAircraft>(AircraftDisplayed))
	{
		FullLoadout.AircraftClass = BaseAir->GetClass();
	}

	// TODO: Change funtions so that they suit this

	TMap<FName, TSubclassOf<ABaseWeapon>> Loadout{};

	for (auto& Pair : EquippedWeapons)
	{
		AActor* Actor = Pair.Value;

		Loadout.Add(Pair.Key, Actor->GetClass());
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
	DEBUG_TIME(100.f, "Debug");
	World->GetTimerManager().ClearAllTimersForObject(this);
	World->GetTimerManager().ClearAllTimersForObject(APC);
	if (IsValid(APC->GetMenuManager())) World->GetTimerManager().ClearAllTimersForObject(APC->GetMenuManager());

	UMissionManagerSubsystem* MissionManager = GI->GetSubsystem<UMissionManagerSubsystem>();
	ULevelTransitionSubsystem* LevelTransition = GI->GetSubsystem<ULevelTransitionSubsystem>();

	if (!IsValid(MissionManager) || !IsValid(LevelTransition)) return;
	DEBUG_TIME(100.f, "Debug");
	const FMissionData& Mission = MissionManager->GetCurrentMission();
	LevelTransition->LoadIntermission(Mission.TakeoffType);
}