// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Aircraft/Player/T38Pawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Units/Aircraft/FlightComponent.h"
#include "Animation/AnimInstance.h"
#include "Player Info/AircraftPlayerController.h"

AT38Pawn::AT38Pawn() 
{
	PrimaryActorTick.bCanEverTick = true;
}

void AT38Pawn::PossessedBy(AController* NewController) 
{
	Super::PossessedBy(NewController);
}

void AT38Pawn::Tick(float DeltaSeconds) 
{
	Super::Tick(DeltaSeconds);
}