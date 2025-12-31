// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("SU!"));
#include "Units/Aircraft/Player/Su25Pawn.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Units/Aircraft/FlightComponent.h"
#include "Player Info/AircraftPlayerController.h"

//Initialize Su25 Player Pawn
ASu25Pawn::ASu25Pawn() 
{
	PrimaryActorTick.bCanEverTick = true;
}
void ASu25Pawn::PossessedBy(AController* NewController) 
{
	Super::PossessedBy(NewController);
}

void ASu25Pawn::BeginPlay()
{
	Super::BeginPlay();
}
