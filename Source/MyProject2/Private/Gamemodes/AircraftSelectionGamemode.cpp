// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Gamemode!"));
#include "Gamemodes/AircraftSelectionGamemode.h"
#include "GameFramework/SpectatorPawn.h"
#include "UI/AircraftSelectionWidget.h"
#include "AircraftPlayerController.h"
#include "Gamemodes/CurrentPlayerState.h"
#include "MenuManagerComponent.h"
#include "Structs and Data/ControlModeTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Structs and Data/MenuState.h"
#include "Kismet/GameplayStatics.h"

AAircraftSelectionGamemode::AAircraftSelectionGamemode() 
{
}

void AAircraftSelectionGamemode::BeginPlay() 
{
	Super::BeginPlay();

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC)  return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ASpectatorPawn* Spectator = GetWorld()->SpawnActor<ASpectatorPawn>(ASpectatorPawn::StaticClass(), FVector::ZeroVector,FRotator::ZeroRotator,SpawnParams);

	if (!Spectator)  return;

	PC->Possess(Spectator);
	APC = Cast<AAircraftPlayerController>(PC);
	PS = Cast<ACurrentPlayerState>(PC->PlayerState);

	if (!APC || !PS) return; 
	APC->SetControlMode(EControlMode::Menu);
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]() 
	{
		APC->MenuManager->AddDatabase();
		APC->MenuManager->ChooseAircraftUI();
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

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FVector PreviewLocation = FVector::ZeroVector;
	FRotator PreviewRotation = FRotator::ZeroRotator;
	AircraftDisplayed = GetWorld()->SpawnActor<APawn>(SpawnIn, PreviewLocation, PreviewRotation, SpawnParams);
}

void AAircraftSelectionGamemode::SpawnInWeapon(TSubclassOf<ABaseWeapon> Weapon, FName Pylon) {
	if (!AircraftDisplayed || (WeaponDisplayed && WeaponDisplayed->GetClass() == Weapon)) return;

	if (WeaponDisplayed) 
	{
		WeaponDisplayed->Destroy();
		WeaponDisplayed = nullptr;
	}

	ABaseAircraft* BaseAircraft = Cast<ABaseAircraft>(AircraftDisplayed);
	if (!BaseAircraft) return;

	USkeletalMeshComponent* Mesh = BaseAircraft->Airframe;

	FTransform SocketTransform = Mesh->GetSocketTransform(Pylon);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	WeaponDisplayed = GetWorld()->SpawnActor<AActor>(Weapon, SocketTransform, SpawnParams);
	if (!WeaponDisplayed) return;

	WeaponDisplayed->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, Pylon);
}