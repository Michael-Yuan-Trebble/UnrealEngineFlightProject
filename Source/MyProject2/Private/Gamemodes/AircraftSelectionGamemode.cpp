// Fill out your copyright notice in the Description page of Project Settings.

#include "Gamemodes/AircraftSelectionGamemode.h"
#include "GameFramework/SpectatorPawn.h"
#include "Player Info/AircraftPlayerController.h"
#include "Units/Components/Player/MenuManagerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player Info/PlayerGameInstance.h"
#include "Subsystem/LevelTransitionSubsystem.h"
#include "Subsystem/MissionManagerSubsystem.h"
#include "Units/Components/Aircraft/WeaponSystemComponent.h"
#include "Units/Aircraft/BaseAircraft.h"
#include "Debug/DebugHelper.h"

AAircraftSelectionGamemode::AAircraftSelectionGamemode() {

}

void AAircraftSelectionGamemode::BeginPlay() {
	Super::BeginPlay();
	
	if (!IsValid(GetWorld())) return;

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
		APC = Cast<AAircraftPlayerController>(PC);

	if (!IsValid(APC)) return;

	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ASpectatorPawn* Spectator = GetWorld()->SpawnActor<ASpectatorPawn>(ASpectatorPawn::StaticClass(), FVector::ZeroVector,FRotator::ZeroRotator,SpawnParams);
	if (!IsValid(Spectator)) return;

	APC->Possess(Spectator);
	APC->SetControlMode(EControlMode::Menu);
	TWeakObjectPtr<AAircraftPlayerController> WeakAPC = APC;

	GetWorld()->GetTimerManager().SetTimerForNextTick([WeakAPC,
		AircraftSelectClass = AircraftSelectClass,
		WeaponSelectClass = WeaponSelectClass,
		BuySelectionClass = BuySelectionClass,
		SpecialSelectionClass = SpecialSelectionClass,
		GreyOutClass = GreyOutClass]() 
		{
			if (!WeakAPC.IsValid()) return;
			AAircraftPlayerController* PC = WeakAPC.Get();
			if (!IsValid(PC)) return;
			UMenuManagerComponent* MenuManager = PC->GetMenuManager();
			if (!IsValid(MenuManager)) return;

			MenuManager->SetupClasses(AircraftSelectClass, WeaponSelectClass, BuySelectionClass, SpecialSelectionClass, GreyOutClass);
			MenuManager->ChooseAircraftUI();
		});
}

void AAircraftSelectionGamemode::SpawnInAircraft(const TSubclassOf<APawn> SpawnIn) {
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
	if (ABaseAircraft* Preview = Cast<ABaseAircraft>(AircraftDisplayed))
		Preview->SetLandingGearVisiblility(true);
}

// TODO: Combine these into one function with a bool to differentiate final logic

void AAircraftSelectionGamemode::SpawnInWeapon(const TSubclassOf<ABaseWeapon> Weapon, const FName& Pylon) {
	if (!IsValid(AircraftDisplayed) || !IsValid(Weapon)) return;

	ABaseAircraft* BaseAircraft = Cast<ABaseAircraft>(AircraftDisplayed);
	if (!IsValid(BaseAircraft)) return;

	UWeaponSystemComponent* WeaponComp = BaseAircraft->GetWeaponComp();
	if (!IsValid(WeaponComp)) return;

	WeaponComp->RemovePylon(Pylon);
	WeaponComp->AddPylon(Pylon, BaseAircraft->GetAirStats()->WeaponInfo.Pylon);

	if (TObjectPtr<AActor>* WeaponPtr = EquippedWeapons.Find(Pylon)) {
		if (IsValid(*WeaponPtr))
			(*WeaponPtr)->Destroy();
	}

	AActor* WeaponDisplayed = WeaponComp->AddWeapon(Pylon, Weapon);
	EquippedWeapons.Add(Pylon, WeaponDisplayed);
}

void AAircraftSelectionGamemode::ClearWeapons(const FName& Pylon) {
	if (!IsValid(AircraftDisplayed)) return;

	ABaseAircraft* BaseAircraft = Cast<ABaseAircraft>(AircraftDisplayed);
	if (!IsValid(BaseAircraft)) return;

	UWeaponSystemComponent* WeaponComp = BaseAircraft->GetWeaponComp();
	if (!IsValid(WeaponComp)) return;

	WeaponComp->RemovePylon(Pylon);

	if (TObjectPtr<AActor>* WeaponPtr = EquippedWeapons.Find(Pylon)) {
		if (IsValid(*WeaponPtr))
			(*WeaponPtr)->Destroy();
	}

	EquippedWeapons.Remove(Pylon);
}

void AAircraftSelectionGamemode::EndSelection(AAircraftPlayerController* Controller) {
	if (!ReadyPlayers.Contains(Controller)) {
		ReadyPlayers.Add(Controller);
		TryAdvanceToNextStage();
	}
}

// TODO: Try to make it so it advances to a "Buffer" screen
void AAircraftSelectionGamemode::TryAdvanceToNextStage() {
	// TODO: Make Co-Op be able to work if implemented
	//if (ReadyPlayers.Num() < PlayersRequired) return;

	UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	UPlayerGameInstance* GI = World->GetGameInstance<UPlayerGameInstance>();
	APlayerController* PC = World->GetFirstPlayerController();
	if (!IsValid(GI) || !IsValid(PC)) return;

	AAircraftPlayerController* LocalAPC = IsValid(APC) ? APC.Get() : Cast<AAircraftPlayerController>(PC);

	if (IsValid(LocalAPC) && IsValid(LocalAPC->GetMenuManager())) 
		LocalAPC->DisableInput(Cast<APlayerController>(PC));
	
	// TODO: See what would work better, returning if there is no aircraft displayed or letting it ride and just using the fallback

	if (ABaseAircraft* BaseAir = Cast<ABaseAircraft>(AircraftDisplayed))
		FullLoadout.AircraftClass = BaseAir->GetClass();

	TMap<FName, TSubclassOf<ABaseWeapon>> Loadout{};

	for (auto& Pair : EquippedWeapons)
		Loadout.Add(Pair.Key, Pair.Value->GetClass());

	FullLoadout.EquippedWeapons = Loadout;
	GI->SetLoadout(FullLoadout);

	// TODO: Set Specials HERE

	Transition();
}

void AAircraftSelectionGamemode::Transition() {
	if (!IsValid(APC) || bFinished || !IsValid(GetWorld())) return;
	bFinished = true;
	APC->ClientMessage(TEXT("COMPLETE"));
	if (auto* GI = GetWorld()->GetGameInstance<UPlayerGameInstance>())
		GI->FadeIn(ELevelType::Transition);
}

void AAircraftSelectionGamemode::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	if (UWorld* World = GetWorld()) {
		for (auto& Pair : EquippedWeapons) {
			if (Pair.Value && Pair.Value->IsValidLowLevel()) {
				Pair.Value->Destroy();
			}
		}
		EquippedWeapons.Empty();

		if (IsValid(AircraftDisplayed)) {
			AircraftDisplayed->Destroy();
			AircraftDisplayed = nullptr;
		}
		World->GetTimerManager().ClearAllTimersForObject(this);
		World->GetTimerManager().ClearAllTimersForObject(APC);
		if (IsValid(APC->GetMenuManager())) World->GetTimerManager().ClearAllTimersForObject(APC->GetMenuManager());
	}

	Super::EndPlay(EndPlayReason);
}