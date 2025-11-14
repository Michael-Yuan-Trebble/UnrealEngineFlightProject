// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Gamemode!"));
#include "Gamemodes/AircraftSelectionGamemode.h"
#include "GameFramework/SpectatorPawn.h"
#include "UI/SelectionUI/AircraftSelectionWidget.h"
#include "AircraftPlayerController.h"
#include "Gamemodes/CurrentPlayerState.h"
#include "Aircraft/MenuManagerComponent.h"
#include "Structs and Data/ControlModeTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Structs and Data/MenuState.h"
#include "Gamemodes/PlayerGameInstance.h"
#include "Aircraft/BaseAircraft.h"

FActorSpawnParameters SpawnParams;

AAircraftSelectionGamemode::AAircraftSelectionGamemode() 
{
}

void AAircraftSelectionGamemode::BeginPlay() 
{
	Super::BeginPlay();

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC)  return;

	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ASpectatorPawn* Spectator = GetWorld()->SpawnActor<ASpectatorPawn>(ASpectatorPawn::StaticClass(), FVector::ZeroVector,FRotator::ZeroRotator,SpawnParams);

	if (!Spectator)  return;

	PC->Possess(Spectator);
	APC = Cast<AAircraftPlayerController>(PC);
	PS = Cast<ACurrentPlayerState>(PC->PlayerState);

	if (!APC || !PS) return; 
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

	FTransform SocketTransform = Mesh->GetSocketTransform(Pylon);
	AActor* WeaponDisplayed;
	if (AActor** WeaponPtr = EquippedWeapons.Find(Pylon)) 
	{
		WeaponDisplayed = *WeaponPtr;
		if (WeaponDisplayed && WeaponDisplayed->IsValidLowLevel())
		{
			WeaponDisplayed->Destroy();
			WeaponDisplayed = nullptr;
		}

		WeaponDisplayed = GetWorld()->SpawnActor<AActor>(Weapon, SocketTransform, SpawnParams);
		WeaponDisplayed->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, Pylon);
		EquippedWeapons[Pylon] = WeaponDisplayed;
	}
	else 
	{
		WeaponDisplayed = GetWorld()->SpawnActor<AActor>(Weapon, SocketTransform, SpawnParams);
		WeaponDisplayed->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, Pylon);
		EquippedWeapons.Add(Pylon, WeaponDisplayed);
	}
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
	if (ReadyPlayers.Num() < PlayersRequired) return;

	UWorld* World = GetWorld();
	if (!World) return;

	UPlayerGameInstance* GI = GetWorld()->GetGameInstance<UPlayerGameInstance>();
	if (!GI) return;

	if (GI->LevelName.IsNone()) return;

	const FString MapPath = FString::Printf(TEXT("/Game/Maps/%s"), *GI->LevelName.ToString());
	if (!FPackageName::DoesPackageExist(MapPath)) return;

	UGameplayStatics::OpenLevel(World, GI->LevelName);
}