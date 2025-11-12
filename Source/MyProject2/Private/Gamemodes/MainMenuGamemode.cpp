// Fill out your copyright notice in the Description page of Project Settings.

#include "Gamemodes/MainMenuGamemode.h"
#include "Kismet/GameplayStatics.h"
#include "AircraftPlayerController.h"
#include "Gamemodes/MainMenuManager.h"

AMainMenuGamemode::AMainMenuGamemode() 
{

}

void AMainMenuGamemode::BeginPlay() 
{
	Super::BeginPlay();

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) return;

	APC = Cast<AAircraftPlayerController>(PC);
	PS = Cast<ACurrentPlayerState>(PC->PlayerState);

	if (!APC || !PS) return;
	APC->SetControlMode(EControlMode::Menu);
}