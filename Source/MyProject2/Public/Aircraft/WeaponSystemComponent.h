// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structs and Data/CooldownWeapon.h"
#include "Structs and Data/Aircraft Data/AircraftStats.h"
#include "WeaponSystemComponent.generated.h"

class APlayerHUD;
class ABaseAircraft;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT2_API UWeaponSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWeaponSystemComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Setup(ABaseAircraft* InBase, UAircraftStats* InStats);

	void FireBullets();

	void ReEquip(FCooldownWeapon& Replace);

	void EquipWeapons();

	void FireWeaponSelected(TSubclassOf<ABaseWeapon> WeaponClass, AActor* Target, float Speed);

	void SelectWeapon(int WeaponIndex);

	void UpdateLockedOn(float DeltaSeconds, AActor* Target);

	void SetWeapons(TMap<FName, TSubclassOf<ABaseWeapon>> In);

	void BuildWeaponGroups();

	void AddPylons();

	bool bLocked;

	float LockTime = 5.f;

	UPROPERTY(BlueprintReadOnly)
	float MaxWeaponCountSelected;

	UPROPERTY(BlueprintReadOnly)
	float CurrentWeaponCount;

	ABaseAircraft* Controlled;

	APlayerHUD* HUD;

	UAircraftStats* AirStats;

	ABaseWeapon* CurrentWeapon;

	TMap<FName, TSubclassOf<ABaseWeapon>> Loadout;

	TMap<TSubclassOf<ABaseWeapon>, TArray<FCooldownWeapon*>> WeaponGroups;

	TMap<FName, UStaticMeshComponent*> PylonSockets;

	TArray<FCooldownWeapon> AvailableWeapons;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AAircraftBullet> Bullet;

private:
	void GetCount();
};
