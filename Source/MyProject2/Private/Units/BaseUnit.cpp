// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Unit!"));
#include "Units/BaseUnit.h"
#include "Player Info/PlayerGameState.h"
#include "Subsystem/AircraftRegistry.h"
#include "Components/BoxComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "UI/PlayerHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Units/HealthComponent.h"

ABaseUnit::ABaseUnit() 
{
	UnitRoot = CreateDefaultSubobject<UBoxComponent>(TEXT("Root"));
	RootComponent = UnitRoot;

	Constraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("Constraint"));
	Constraint->SetupAttachment(UnitRoot);

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	PrimaryActorTick.bCanEverTick = true;
	if (IsValid(HealthComp))
	{
		HealthComp->OnDeath.AddDynamic(this, &ABaseUnit::HandleDestroyed);
	}
}

void ABaseUnit::BeginPlay()
{
	Super::BeginPlay();
	if (UWorld* World = GetWorld()) Registry = UAircraftRegistry::Get(World);
	if (bStartsTargetable) ActivateTarget();
	if (IsValid(HealthComp)) HealthComp->Setup(health);
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
	if (!IsValid(HealthComp)) return;
	HealthComp->ApplyDamage(Damage, Weapon, Launcher, Target);
}

void ABaseUnit::HandleDestroyed(AActor* Weapon, AActor* Launcher, AActor* Target)
{
	OnUnitDeath.Broadcast();
	APlayerGameState* GS = GetWorld()->GetGameState<APlayerGameState>();
	if (IsValid(GS))
	{
		GS->RegisterKill(Launcher, Target, Weapon->GetClass());
	}
	APlayerHUD* HUD = Cast<APlayerHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
	if (IsValid(HUD))
	{
		HUD->OnUnitDestroyed(this);
	}

	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
	for (auto* Comp : GetComponents()) 
	{
		if (IsValid(Comp)) Comp->SetComponentTickEnabled(false);
	}
	DeactivateTarget();
	Destroy();
}

void ABaseUnit::HandleLOD(FVector CameraLoc) {}

void ABaseUnit::ActivateTarget() 
{
	if (!IsValid(Registry) || bIsTargetable) return;
	bIsTargetable = true;
	Registry->Register(this);
}

void ABaseUnit::DeactivateTarget()
{
	if (!IsValid(Registry) || !bIsTargetable) return;
	bIsTargetable = false;
	Registry->Unregister(this);
}