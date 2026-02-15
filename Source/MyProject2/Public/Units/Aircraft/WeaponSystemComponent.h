// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structs and Data/CooldownWeapon.h"
#include "Structs and Data/Aircraft Data/AircraftStats.h"
#include "WeaponSystemComponent.generated.h"

// TODO: For now cone angle is like this, maybe change it in the future
#define CONE_ANGLE 30.f
// TODO: Make this variable maybe?
#define LOCKTIME 1.f

class ABaseAircraft;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnWeaponCountUpdated,FName, WeaponName, int32, CurrentCount, int32, MaxCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHUDLockedOn, float, LockPercent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponSystemWeaponHitResult, bool, bHit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHUDBulletHit);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT2_API UWeaponSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	FWeaponSystemWeaponHitResult OnWeaponHit;

	FOnHUDLockedOn OnHUDLockedOn;

	FOnHUDBulletHit OnHUDBulletHit;

	UWeaponSystemComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Setup(ABaseAircraft* InBase, UAircraftStats* InStats);

	void FireBullets();

	void ReEquip(FCooldownWeapon& Replace);

	void EquipWeapons();

	void FireWeaponSelected(const TSubclassOf<ABaseWeapon> WeaponClass, AActor* Target, const float Speed);

	void SelectWeapon(const int WeaponIndex);

	void UpdateLockedOn(const float DeltaSeconds, AActor* Target);

	void SetWeapons(TMap<FName, TSubclassOf<ABaseWeapon>> In);

	void BuildWeaponGroups();

	void AddPylons();

	void GetCount();

	void SearchAndEquipWeapon(const TSubclassOf<ABaseWeapon> WeaponClass);

	UFUNCTION()
	void OnWeaponResult(bool bHit);

	bool bLocked = false;

	float LockTime = 5.f;

	UPROPERTY(BlueprintReadOnly)
	float MaxWeaponCountSelected = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float CurrentWeaponCount = 0.f;

	UPROPERTY(BlueprintAssignable)
	FOnWeaponCountUpdated OnWeaponCountUpdated;

	UPROPERTY()
	ABaseAircraft* Controlled = nullptr;

	UPROPERTY()
	UAircraftStats* AirStats = nullptr;

	UPROPERTY()
	ABaseWeapon* CurrentWeapon = nullptr;

	UPROPERTY()
	TMap<FName, TSubclassOf<ABaseWeapon>> Loadout;

	TMap<TSubclassOf<ABaseWeapon>, TArray<FCooldownWeapon*>> WeaponGroups;

	UPROPERTY()
	TMap<FName, UStaticMeshComponent*> PylonSockets;

	UPROPERTY()
	TArray<FName> EquippedWeaponNames;

	UPROPERTY()
	TArray<FCooldownWeapon> AvailableWeapons;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AAircraftBullet> Bullet = nullptr;

	ABaseWeapon* GetWeapon() const { return CurrentWeapon; };

	void ResetLockedOn();

private:

};
