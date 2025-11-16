// Fill out your copyright notice in the Description page of Project Settings.


#include "Specials/Countermeasures.h"
#include "Kismet/GameplayStatics.h"

void UCountermeasures::ActivateSpecial() {
	if (!FlareActorClass || !OwningAircraft) return;

	UWorld* World = OwningAircraft->GetWorld();
	if (!World) return;

	USkeletalMeshComponent* Mesh = OwningAircraft->Airframe;

	FName FlareSocket = "FlareSocket";

	if (!Mesh->DoesSocketExist(FlareSocket)) return;

	FTransform SocketTransform = Mesh->GetSocketTransform(FlareSocket);

	FVector FlareLocation = SocketTransform.GetLocation();
	FQuat FlareRotation = SocketTransform.GetRotation();
	FRotator FlareRotator = FlareRotation.Rotator();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwningAircraft;
	SpawnParams.Instigator = OwningAircraft->GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	World->SpawnActor<AActor>(FlareActorClass, FlareLocation, FlareRotator, SpawnParams);
}