// Fill out your copyright notice in the Description page of Project Settings.

#include "Units/Components/Aircraft/WeaponSystemComponent.h"
#include "Units/Aircraft/BaseAircraft.h"
#include "Weapons/AircraftBullet.h"
#include "Units/Components/Aircraft/RadarComponent.h"
#include "Components/BoxComponent.h"
#include "Units/BaseUnit.h"
#include "Kismet/GameplayStatics.h"
#include "UI/PlayerHUD.h"
#include "Interfaces/LockableTarget.h"
#include "Structs and Data/MathLib/FlightMathLibrary.h"
#include "Debug/DebugHelper.h"

UWeaponSystemComponent::UWeaponSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWeaponSystemComponent::Setup(ABaseAircraft* InBase, const UAircraftStats* InStats)
{
	Controlled = InBase;
	if (IsValid(Controlled)) {
		BulletStats = Controlled->GetBulletStats();
		Airframe = Controlled->GetAirframe();
		AirWeaponInfo = InStats->WeaponInfo;
	}
}

void UWeaponSystemComponent::FireBullets()
{
	if (!IsValid(Controlled) || !IsValid(BulletStats) || !IsValid(BulletStats->BulletClass) || !IsValid(Airframe)) return;
	
	int8 GunI = 0;
	while (true) 
	{
		const FName SocketName = FName(*FString::Printf(TEXT("Gun%d"), GunI));

		if (!IsValid(Airframe) || !Airframe->DoesSocketExist(SocketName)) break;

		GunI++;

		FVector MuzzleLocation = Airframe->GetSocketLocation(SocketName);
		FRotator MuzzleRotation = Airframe->GetSocketRotation(SocketName);
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = Controlled;
		SpawnParams.Instigator = Controlled;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AAircraftBullet* SpawnBullet = GetWorld()->SpawnActor<AAircraftBullet>(BulletStats->BulletClass, MuzzleLocation, MuzzleRotation, SpawnParams);
		if (!SpawnBullet) return;
		SpawnBullet->FireInDirection(MuzzleRotation.Vector());
	}
}

void UWeaponSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsValid(Controlled)) return;

	// ====================================
	// Weapon refresh to check respawning conditions
	// ====================================

	for (FCooldownWeapon& Weapon : AvailableWeapons) 
	{
		if (!Weapon.CanFire()) 
		{
			Weapon.UpdateCooldown(DeltaTime);
			if (Weapon.CanFire()) 
			{
				ReEquip(Weapon);
			}
		}
	}
	UpdateLockedOn(DeltaTime, Controlled->GetRadarComp()->GetSelected());
}

void UWeaponSystemComponent::SetWeapons(TMap<FName, TSubclassOf<ABaseWeapon>> In) 
{
	Loadout = In;
	AddPylons();
	EquipWeapons();
}

void UWeaponSystemComponent::AddPylons() 
{
	for (int i = 0; i < AirWeaponInfo.NumOfPylons; i++)
	{
		FName SocketName = FName(*FString::Printf(TEXT("Pylon_%d"), i));
		AddPylon(SocketName, AirWeaponInfo.Pylon);
	}
}

void UWeaponSystemComponent::AddPylon(const FName& PylonName, UStaticMesh* PylonMesh) {
	UStaticMeshComponent* TempPylon = NewObject<UStaticMeshComponent>(this);
	if (TempPylon && IsValid(PylonMesh)) {
		if (!IsValid(Airframe)) return;
		FTransform SocketTransform = Airframe->GetSocketTransform(PylonName, RTS_World);
		SocketTransform.SetScale3D(FVector(1.f));
		TempPylon->SetStaticMesh(PylonMesh);
		TempPylon->SetWorldTransform(SocketTransform);
		TempPylon->RegisterComponent();
		TempPylon->AttachToComponent(Airframe, FAttachmentTransformRules::SnapToTargetNotIncludingScale, PylonName);
		TempPylon->AddLocalOffset(FVector(0, -80, -50));
		PylonSockets.Add(PylonName, TempPylon);
	}
}

void UWeaponSystemComponent::RemovePylon(const FName& PylonName) {
	if (TObjectPtr<UStaticMeshComponent>* Found = PylonSockets.Find(PylonName)) {
		if (IsValid(*Found)) (*Found)->DestroyComponent();
		PylonSockets.Remove(PylonName);
	}
}

AActor* UWeaponSystemComponent::AddWeapon(const FName& PylonName, TSubclassOf<ABaseWeapon> Weapon) {
	UStaticMeshComponent* PylonComp = PylonSockets.FindRef(PylonName);
	if (!IsValid(PylonComp)) return nullptr;

	FTransform SocketTransform = PylonComp->GetSocketTransform(FName("Socket"));
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Controlled;

	ABaseWeapon* SpawnIn = GetWorld()->SpawnActor<ABaseWeapon>(Weapon, SocketTransform, SpawnParams);
	if (!IsValid(SpawnIn)) return nullptr;
	SpawnIn->Collision->SetSimulatePhysics(false);
	SpawnIn->AttachToComponent(PylonComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Socket"));
	return SpawnIn;
}

void UWeaponSystemComponent::EquipWeapons()
{
	for (const TPair<FName, TSubclassOf<ABaseWeapon>>&Pair : Loadout)
	{
		ABaseWeapon* SpawnIn = Cast<ABaseWeapon>(AddWeapon(Pair.Key, Pair.Value));
		if (!IsValid(SpawnIn)) continue;

		FCooldownWeapon tempCool;
		tempCool.Init(Pair.Value, SpawnIn, Pair.Key, SpawnIn->GetCooldown(), SpawnIn->GetRange());
		tempCool.ResetFire();
		tempCool.SetTargetType(SpawnIn->GetTargetTypes());
		AvailableWeapons.Add(tempCool);
	}
	// Defer this, would be 0 otherwise, let BeginPlay go through
	FTimerHandle TH;
	GetWorld()->GetTimerManager().SetTimer(TH, this, &UWeaponSystemComponent::BuildWeaponGroups, 0.01f, false);
}

//Create and Replace Missile in Array
void UWeaponSystemComponent::ReEquip(FCooldownWeapon& Replace)
{
	UStaticMeshComponent* PylonComp = PylonSockets.FindRef(Replace.SocketName);
	if (!IsValid(PylonComp)) return;
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Controlled;
	Replace.WeaponInstance = GetWorld()->SpawnActor<ABaseWeapon>(Replace.WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	if (!Replace.WeaponInstance) return;
	FTransform SocketTransform = PylonComp->GetSocketTransform(FName("Socket"));
	Replace.WeaponInstance->AttachToComponent(PylonComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Socket"));
	Replace.ResetFire();
	GetCount();
}

void UWeaponSystemComponent::BuildWeaponGroups() 
{
	WeaponGroups.Empty();
	EquippedWeaponNames.Empty();

	for (FCooldownWeapon& CW : AvailableWeapons)
	{
		if (!CW.WeaponInstance) continue;
		TSubclassOf<ABaseWeapon> WeaponClass = CW.WeaponClass;

		if (!WeaponGroups.Contains(WeaponClass)) 
		{
			WeaponGroups.Add(WeaponClass, TArray<FCooldownWeapon*>());
			EquippedWeaponNames.Add(CW.WeaponInstance->GetName());
		}
		WeaponGroups[WeaponClass].Add(&CW);
	}
	if (WeaponGroups.Num() > 0) SelectWeapon(0);
}

void UWeaponSystemComponent::FireWeaponSelected(AActor* Target, const float Speed)
{
	if (!WeaponGroups.Contains(CurrentWeaponClass)) return;
	for (FCooldownWeapon* Weapon : WeaponGroups[CurrentWeaponClass])
	{
		if (!Weapon || !IsValid(Weapon->WeaponInstance) || !Weapon->CanFire()) continue;

		Weapon->WeaponInstance->OnWeaponResult.AddDynamic(this, &UWeaponSystemComponent::OnWeaponResult);

		if (Target && bLocked && IsValid(Controlled)) {
			Weapon->WeaponInstance->FireTracking(Controlled->GetUnitSpeed(), Target);
		}
		else 
			Weapon->WeaponInstance->FireStatic(Speed);

		Weapon->StartCooldown();
		GetCount();
		return;
	}
}

void UWeaponSystemComponent::OnWeaponResult(bool bHit) 
{
	OnWeaponHit.Broadcast(bHit);
}

void UWeaponSystemComponent::SelectWeapon(const int WeaponIndex)
{
	TArray<TSubclassOf<ABaseWeapon>> Keys{};
	WeaponGroups.GetKeys(Keys);

	if (!Keys.IsValidIndex(WeaponIndex)) return;

	TSubclassOf<ABaseWeapon> SelectedClass = Keys[WeaponIndex];

	const TArray<FCooldownWeapon*>* WeaponArray = WeaponGroups.Find(SelectedClass);
	if (!WeaponArray || WeaponArray->Num() <= 0) return;

	const FCooldownWeapon* WeaponData = (*WeaponArray)[0];
	if (!WeaponData) return;

	CurrentTargetingTypes = WeaponData->GetTargetTypes();
	CurrentRange = WeaponData->Range;

	ResetLockedOn();

	CurrentWeapon = (*WeaponArray)[0]->WeaponInstance;
	CurrentWeaponClass = (*WeaponArray)[0]->WeaponClass;
	if (IsValid(CurrentWeapon)) CurrentWeaponName = CurrentWeapon->GetName();
	GetCount();
}

// For AI to find a weapon
void UWeaponSystemComponent::SearchAndEquipWeapon(const TSubclassOf<ABaseWeapon> WeaponClass) 
{
	if (IsValid(CurrentWeapon) && WeaponClass == CurrentWeapon->GetClass()) return;
	const TArray<FCooldownWeapon*>* WeaponArray = WeaponGroups.Find(WeaponClass);
	if (!WeaponArray || WeaponArray->Num() == 0) return;
	
	const FCooldownWeapon* WeaponData = (*WeaponArray)[0];
	if (!WeaponData || !WeaponData->WeaponInstance) return;

	ResetLockedOn();

	CurrentWeapon = (*WeaponArray)[0]->WeaponInstance;
	CurrentWeaponClass = (*WeaponArray)[0]->WeaponClass;
}

void UWeaponSystemComponent::GetCount() 
{
	if (!IsValid(CurrentWeaponClass)) return;

	const TArray<FCooldownWeapon*>* WeaponArray = WeaponGroups.Find(CurrentWeaponClass);
	if (!WeaponArray) return;

	MaxWeaponCountSelected = WeaponArray->Num();
	CurrentWeaponCount = 0;

	for (const FCooldownWeapon* Weapon : *WeaponArray)
	{
		if (Weapon && Weapon->CanFire())
			CurrentWeaponCount++;
	}

	OnWeaponCountUpdated.Broadcast(CurrentWeaponName, CurrentWeaponCount, MaxWeaponCountSelected);
	if (CurrentWeaponCount <= 0) ResetLockedOn();
}

void UWeaponSystemComponent::UpdateLockedOn(const float DeltaSeconds, ABaseUnit* Target)
{
	if (CurrentWeaponCount <= 0) return;

	if (!CurrentWeaponClass || !IsValid(Target))
	{
		if (bLocked || LockTime > 0.f) 
			ResetLockedOn();
		else 
			// TODO: Change this to not broadcast every tick
			OnHUDLockedOn.Broadcast(false);
		return;
	}

	ABaseWeapon* TemporaryWeapon = CurrentWeaponClass->GetDefaultObject<ABaseWeapon>();
	if (!IsValid(TemporaryWeapon)) return;

	if (!TemporaryWeapon->GetCanLock() || CurrentWeaponCount <= 0)
	{
		if (bLocked || LockTime > 0.f) 
			ResetLockedOn();
		return;
	}

	ILockableTarget* LockTarget = Cast<ILockableTarget>(Target);
	if (!LockTarget) 
	{
		ResetLockedOn();
		return;
	}

	if (!CurrentTargetingTypes.Contains(LockTarget->GetTargetType()))
	{
		ResetLockedOn();
		return;
	}

	if (!IsValid(Controlled)) return;
	FVector ToTarget = Target->GetActorLocation() - Controlled->GetActorLocation();
	float Distance = ToTarget.Size();

	if (UFlightMathLibrary::SpeedToKMH(Distance) > UFlightMathLibrary::SpeedToKMH(CurrentRange))
	{
		ResetLockedOn();
		return;
	}

	if (!IsValid(Airframe)) return;

	ToTarget.Normalize();
	float Dot = FVector::DotProduct(Airframe->GetForwardVector(), ToTarget);
	bool bInCone = Dot > FMath::Cos(FMath::DegreesToRadians(ConeAngle));
	bool bWasLocked = bLocked;
	if (bInCone)
	{
		LockTime += DeltaSeconds;

		// TODO: Make it variable
		bLocked = LockTime >= MaxLockTime;
	}
	else
	{
		bLocked = false;
		LockTime = 0.f;
	}
	if (bLocked && !bWasLocked) if (ABaseAircraft* Aircraft = Cast<ABaseAircraft>(Target)) Aircraft->OnLockedOnByEnemy.Broadcast();

	float LockPercent = 0.f;
	if (MaxLockTime == 0) 
		LockPercent = 1.f;
	else 
		LockPercent = FMath::Clamp(LockTime / MaxLockTime, 0.f, 1.f);
	AIR_DEBUG_KEY(0, FColor::Green, "%f", LockPercent);

	if ((PreviousLockPercent == 0 && LockPercent != 0) || (PreviousLockPercent != LockPercent)) {
		OnLockingSound.Broadcast(LockPercent);
		OnHUDLockedOn.Broadcast(LockPercent);
	}
	PreviousLockPercent = LockPercent;
}

void UWeaponSystemComponent::ResetLockedOn() 
{
	float lastLock = LockTime;
	LockTime = 0.f;
	bLocked = false;
	if (lastLock != 0.f) OnHUDLockedOn.Broadcast(0.f);
}

FCooldownWeapon* UWeaponSystemComponent::GetBestWeaponRange(float Distance) {
	FCooldownWeapon* BestWeapon = nullptr;
	float BestRange = 0.f;
	for (FCooldownWeapon& Weapon : AvailableWeapons) {
		if (!IsValid(Weapon.WeaponInstance)) continue;
		float WeaponRange = Weapon.WeaponInstance->GetRange();
		if (WeaponRange >= Distance && WeaponRange < BestRange) {
			BestRange = WeaponRange;
			BestWeapon = &Weapon;
		}
	}
	return BestWeapon;
}