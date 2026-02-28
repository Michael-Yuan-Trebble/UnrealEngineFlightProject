// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/BaseWeapon.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

ABaseWeapon::ABaseWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ABaseWeapon::FireStatic(const float speed)
{

 }

void ABaseWeapon::FireTracking(const float speed, AActor* Target)
{

 }