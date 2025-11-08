// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseUnit.h"

ABaseUnit::ABaseUnit()
{
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	SetRootComponent(Collision);

	Collision->SetCollisionProfileName(TEXT("Pawn"));
	Collision->SetNotifyRigidBodyCollision(true);
	Collision->SetGenerateOverlapEvents(true);
	Collision->SetBoxExtent(FVector(30.f));

	PrimaryActorTick.bCanEverTick = true;

}

void ABaseUnit::BeginPlay()
{
	Super::BeginPlay();
	isAlive = true;
	if (UWorld* World = GetWorld()) Registry = AAircraftRegistry::Get(World);
	if (bStartsTargetable) ActivateTarget();
}

void ABaseUnit::EndPlay(const EEndPlayReason::Type EndPlayReason) 
{
	DeactivateTarget();
	Super::EndPlay(EndPlayReason);
}

void ABaseUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseUnit::PossessedBy(AController* NewController) 
{
	Super::PossessedBy(NewController);
}

void ABaseUnit::OnDamage_Implementation(AActor* Weapon, float Damage) 
{
	health -= Damage;
	if (health <= 0.f && !isAlive)
	{
		DeactivateTarget();
		Destroy();
	}
}

void ABaseUnit::ActivateTarget() 
{
	if (!Registry || bIsTargetable) return;
	bIsTargetable = true;
	Registry->Register(this);
}

void ABaseUnit::DeactivateTarget()
{
	if (!Registry || !bIsTargetable) return;
	bIsTargetable = false;
	Registry->Unregister(this);
}