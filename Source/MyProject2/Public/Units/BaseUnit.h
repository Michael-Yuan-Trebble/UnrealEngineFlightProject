// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"
#include "Interfaces/LockableTarget.h"
#include "Interfaces/DamageableInterface.h"
#include "Interfaces/TeamInterface.h"
#include "Subsystem/AircraftRegistry.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "BaseUnit.generated.h"

class UHealthComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnThisDeath);

UCLASS()
class MYPROJECT2_API ABaseUnit : public APawn, public ILockableTarget, public IDamageableInterface, public ITeamInterface
{
	GENERATED_BODY()

public:
	ABaseUnit();

	FOnThisDeath OnUnitDeath;

	virtual void OnDamage_Implementation(AActor* Weapon, AActor* Launcher, AActor* Target, float Damage) override;

	virtual void PossessedBy(AController* Controller) override;

	void ActivateTarget();

	void DeactivateTarget();

	UFUNCTION()
	virtual void HandleDestroyed(AActor* Weapon, AActor* Launcher, AActor* Target);

	virtual void HandleLOD(FVector CameraLoc);

// Getters/Setters
public:

	FVector GetTargetLocation() const { return this->GetActorLocation(); };

	bool IsLockable() const { return bAlive && bIsTargetable; };

	ETargetType GetTargetType() const { return UnitType; };

	EFaction GetFaction_Implementation() const { return Faction; };

	void SetTracking(AActor* InTrack) { Tracked = InTrack; };

	AActor* GetTracking() const { return Tracked; };

	float GetHealth() const { return health; };

	EFaction GetFaction() const { return Faction; };

protected:

	UPROPERTY(EditAnywhere)
	UBoxComponent* UnitRoot = nullptr;

	UPROPERTY(EditAnywhere)
	EFaction Faction = EFaction::Ally;

	UPROPERTY(EditAnywhere)
	ETargetType UnitType = ETargetType::Unknown;

	UPROPERTY()
	UPhysicsConstraintComponent* Constraint = nullptr;

	UPROPERTY()
	UAircraftRegistry* Registry = nullptr;

	UPROPERTY()
	UHealthComponent* HealthComp = nullptr;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere)
	bool bStartsTargetable = true;

	UPROPERTY(EditAnywhere)
	bool bIsTargetable = false;

	UPROPERTY(EditAnywhere)
	float health = 100;

	bool bAlive = true;

	UPROPERTY()
	AActor* Tracked = nullptr;

};
