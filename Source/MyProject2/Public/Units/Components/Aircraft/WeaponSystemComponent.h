// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structs and Data/InGame/CooldownWeapon.h"
#include "Structs and Data/Aircraft Data/AircraftStats.h"
#include "Structs and Data/LoadoutInfo/AircraftWeaponInfo.h"
#include "Structs and Data/Weapon Data/BulletStats.h"
#include "Structs and Data/InGame/InGameAirStats.h"
#include "Enums/TargetTypes.h"
#include "WeaponSystemComponent.generated.h"

class ABaseAircraft;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnWeaponCountUpdated,FName, WeaponName, int32, CurrentCount, int32, MaxCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHUDLockedOn, float, LockPercent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponSystemWeaponHitResult, bool, bHit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHUDBulletHit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLockingSound, float, LockPercent);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT2_API UWeaponSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	FWeaponSystemWeaponHitResult OnWeaponHit;

	FOnHUDLockedOn OnHUDLockedOn;

	FOnHUDBulletHit OnHUDBulletHit;

	UPROPERTY(BlueprintAssignable)
	FOnWeaponCountUpdated OnWeaponCountUpdated;

	FOnLockingSound OnLockingSound;

	UWeaponSystemComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Setup(ABaseAircraft* InBase, const UAircraftStats* InStats);

	void FireBullets();

	void FireWeaponSelected(AActor* Target, const float Speed);

	void SelectWeapon(const int WeaponIndex);

	void UpdateLockedOn(const float DeltaSeconds, class ABaseUnit* Target);

	void SetWeapons(TMap<FName, TSubclassOf<ABaseWeapon>> In);

	void SearchAndEquipWeapon(const TSubclassOf<ABaseWeapon> WeaponClass);

	UFUNCTION()
	void OnWeaponResult(bool bHit);

	ABaseWeapon* GetWeapon() const { return CurrentWeapon; };

	TSubclassOf<ABaseWeapon> GetWeaponClass() const{ return CurrentWeaponClass; };

	TMap<TSubclassOf<ABaseWeapon>, TArray<FCooldownWeapon*>> GetWeaponGroups() const { return WeaponGroups; };

	bool GetLocked() const { return bLocked; };

	float GetMaxWeaponCount() const { return MaxWeaponCountSelected; };

	float GetCurrentWeaponCount() const { return CurrentWeaponCount; };

	void ResetLockedOn();

	FCooldownWeapon* GetBestWeaponRange(float Distance);

	void AddPylon(const FName& PylonName, UStaticMesh* PylonMesh);

	void RemovePylon(const FName& PylonName);

	AActor* AddWeapon(const FName& PylonName, TSubclassOf<ABaseWeapon> Weapon);

private:

	void AddPylons();

	void EquipWeapons();

	void GetCount();

	void BuildWeaponGroups();

	void ReEquip(FCooldownWeapon& Replace);

	static constexpr float ConeAngle = 30.f;

	static constexpr float MaxLockTime = 1.f;

	bool bLocked = false;

	bool bPlayingLocking = false;

	float LockTime = 5.f;

	float MaxWeaponCountSelected = 0.f;

	float CurrentWeaponCount = 0.f;

	float PreviousLockPercent = 0.f;

	float CurrentRange = 0.f;

	FName CurrentWeaponName = NAME_None;

	FAircraftWeaponInfo AirWeaponInfo{};

	TArray<ETargetType> CurrentTargetingTypes{};

	UPROPERTY()
	TObjectPtr<ABaseAircraft> Controlled = nullptr;

	UPROPERTY()
	TObjectPtr<UBulletStats> BulletStats = nullptr;

	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> Airframe = nullptr;

	UPROPERTY()
	TObjectPtr<ABaseWeapon> CurrentWeapon = nullptr;

	UPROPERTY()
	TSubclassOf<ABaseWeapon> CurrentWeaponClass = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AAircraftBullet> Bullet = nullptr;

	UPROPERTY()
	TMap<FName, TSubclassOf<ABaseWeapon>> Loadout{};

	UPROPERTY()
	TMap<FName, TObjectPtr<UStaticMeshComponent>> PylonSockets{};

	TMap<TSubclassOf<ABaseWeapon>, TArray<FCooldownWeapon*>> WeaponGroups{};

	UPROPERTY()
	TArray<FName> EquippedWeaponNames{};

	UPROPERTY()
	TArray<FCooldownWeapon> AvailableWeapons{};

	UPROPERTY()
	class URadarComponent* RadarComp = nullptr;
};
