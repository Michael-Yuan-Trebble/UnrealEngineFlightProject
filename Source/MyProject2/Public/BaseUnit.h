// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"
#include "Structs and Data/LockableTarget.h"
#include "Structs and Data/DamageableInterface.h"
#include "Structs and Data/TeamInterface.h"
#include "AircraftRegistry.h"
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

	UPROPERTY(EditAnywhere)
	UBoxComponent* Collision;

	UPROPERTY(EditAnywhere)
	EFaction Faction;

	UPROPERTY(EditAnywhere)
	ETargetType UnitType = ETargetType::Unknown;

	UPROPERTY(EditAnywhere)
	bool bStartsTargetable = true;

	UPROPERTY(EditAnywhere)
	bool bIsTargetable = false;

	UPROPERTY()
	UAircraftRegistry* Registry = nullptr;

	bool isAlive = true;

	float health;

	UHealthComponent* HealthComp;

	virtual FVector GetTargetLocation() const override { return this->GetActorLocation(); };

	virtual bool IsLockable() const override { return isAlive && bIsTargetable; };

	virtual ETargetType GetTargetType() const override { return UnitType; };

	virtual EFaction GetFaction_Implementation() const override{ return Faction; };

	virtual void OnDamage_Implementation(AActor* Weapon, AActor* Launcher, AActor* Target, float Damage) override;

	virtual void PossessedBy(AController* Controller) override;

	void ActivateTarget();

	void DeactivateTarget();

	UFUNCTION()
	virtual void HandleDestroyed(AActor* Weapon, AActor* Launcher, AActor* Target);

	virtual void HandleLOD(FVector CameraLoc);

	AActor* Tracked;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

};
