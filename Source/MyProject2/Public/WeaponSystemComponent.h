// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structs and Data/CooldownWeapon.h"
#include "Structs and Data/AircraftStats.h"
#include "WeaponSystemComponent.generated.h"

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

	void ReEquip(FCooldownWeapon* Replace);

	void EquipWeapons(TArray<TSubclassOf<ABaseWeapon>> WeaponClasses);

	void FireWeaponNotSelected(int WeaponIndex);

	void FireWeaponSelected(int WeaponIndex, AActor* Target);

	void SelectWeapon(int WeaponIndex);

	ABaseAircraft* Controlled;

	UAircraftStats* AirStats;

	TArray<FName> PylonSockets;

	ABaseWeapon CurrentWeapon;

	TArray<FCooldownWeapon> AvailableWeapons;

	TSubclassOf<AActor> Bullet;
};
