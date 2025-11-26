// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Unit!"));
#include "BaseUnit.h"
#include "Gamemodes/PlayerGameState.h"
#include "HealthComponent.h"

ABaseUnit::ABaseUnit()
{
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	SetRootComponent(Collision);

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	Collision->SetCollisionProfileName(TEXT("Pawn"));
	Collision->SetNotifyRigidBodyCollision(true);
	Collision->SetGenerateOverlapEvents(true);
	Collision->SetBoxExtent(FVector(30.f));

	PrimaryActorTick.bCanEverTick = true;
	if (HealthComp) 
	{
		HealthComp->OnDeath.AddDynamic(this, &ABaseUnit::HandleDestroyed);
		HealthComp->OnDamage.AddDynamic(this, &ABaseUnit::HandleHit);
	}
}

void ABaseUnit::BeginPlay()
{
	Super::BeginPlay();
	isAlive = true;
	if (UWorld* World = GetWorld()) Registry = AAircraftRegistry::Get(World);
	if (bStartsTargetable) ActivateTarget();
	HealthComp->Setup(health);
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

void ABaseUnit::OnDamage_Implementation(AActor* Weapon, AActor* Launcher, AActor* Target, float Damage)
{
	HealthComp->ApplyDamage(Damage, Weapon, Launcher, Target);
}

void ABaseUnit::HandleDestroyed(AActor* Weapon, AActor* Launcher, AActor* Target)
{
	APlayerGameState* GS = GetWorld()->GetGameState<APlayerGameState>();
	if (GS)
	{
		GS->RegisterKill(Launcher, Target, Weapon->GetClass());
	}
	DeactivateTarget();
}

void ABaseUnit::HandleHit() {

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