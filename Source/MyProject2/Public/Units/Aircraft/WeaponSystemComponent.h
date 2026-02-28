// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structs and Data/CooldownWeapon.h"
#include "Structs and Data/Aircraft Data/AircraftStats.h"
#include "Structs and Data/InGameAirStats.h"
#include "WeaponSystemComponent.generated.h"

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

	UPROPERTY(BlueprintAssignable)
	FOnWeaponCountUpdated OnWeaponCountUpdated;

	UWeaponSystemComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Setup(ABaseAircraft* InBase, UAircraftStats* InStats);

	void FireBullets();

	void ReEquip(FCooldownWeapon& Replace);

	void FireWeaponSelected(const TSubclassOf<ABaseWeapon> WeaponClass, AActor* Target, const float Speed);

	void SelectWeapon(const int WeaponIndex);

	void UpdateLockedOn(const float DeltaSeconds, class ABaseUnit* Target);

	void SetWeapons(TMap<FName, TSubclassOf<ABaseWeapon>> In);

	void BuildWeaponGroups();

	void GetCount();

	void SearchAndEquipWeapon(const TSubclassOf<ABaseWeapon> WeaponClass);

	UFUNCTION()
	void OnWeaponResult(bool bHit);

	ABaseWeapon* GetWeapon() const { return CurrentWeapon; };

	TMap<TSubclassOf<ABaseWeapon>, TArray<FCooldownWeapon*>> GetWeaponGroups() const { return WeaponGroups; };

	bool GetLocked() const { return bLocked; };

	float GetMaxWeaponCount() const { return MaxWeaponCountSelected; };

	float GetCurrentWeaponCount() const { return CurrentWeaponCount; };

	void ResetLockedOn();

private:

	void AddPylons();

	void EquipWeapons();

	static constexpr float ConeAngle = 30.f;

	static constexpr float MaxLockTime = 1.f;

	bool bLocked = false;

	float LockTime = 5.f;

	float MaxWeaponCountSelected = 0.f;

	float CurrentWeaponCount = 0.f;

	UPROPERTY()
	TObjectPtr<ABaseAircraft> Controlled = nullptr;

	UPROPERTY()
	TObjectPtr<UBulletStats> BulletStats = nullptr;

	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> Airframe = nullptr;

	UPROPERTY()
	TObjectPtr<ABaseWeapon> CurrentWeapon = nullptr;

	UPROPERTY()
	TMap<FName, TSubclassOf<ABaseWeapon>> Loadout{};

	TMap<TSubclassOf<ABaseWeapon>, TArray<FCooldownWeapon*>> WeaponGroups{};

	UPROPERTY()
	TMap<FName, UStaticMeshComponent*> PylonSockets{};

	UPROPERTY()
	TArray<FName> EquippedWeaponNames{};

	UPROPERTY()
	TArray<FCooldownWeapon> AvailableWeapons{};

	UPROPERTY(EditAnywhere)
	TSubclassOf<AAircraftBullet> Bullet = nullptr;

	FInGameAirStats InGameAirStats;
};
