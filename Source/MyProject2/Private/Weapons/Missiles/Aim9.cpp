// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Missiles/Aim9.h"

AAim9::AAim9() {
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Missile"));
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	WeaponMesh->SetupAttachment(RootComponent);

	WeaponName = "Aim-9";

	//static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/Weapons/Aim-9L/Aim-9L_LOD0.Aim-9L_LOD0"));
	//if (MeshAsset.Succeeded()) 
	//{
	//	WeaponMesh->SetStaticMesh(MeshAsset.Object);
	//}
}