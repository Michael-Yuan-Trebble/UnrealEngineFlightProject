// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Weapon Component!"));
#include "Aircraft/WeaponSystemComponent.h"
#include "Aircraft/BaseAircraft.h"
#include "Weapons/AircraftBullet.h"
#include "DrawDebugHelpers.h"

UWeaponSystemComponent::UWeaponSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWeaponSystemComponent::Setup(ABaseAircraft* InBase, UAircraftStats* InStats) 
{
	Controlled = InBase;
	AirStats = InStats;
}

void UWeaponSystemComponent::FireBullets()
{
	//Spawns a bullet actor whilst firing at socket
	if (!Bullet || !(Controlled->Airframe->DoesSocketExist("Gun"))) return;

	FVector MuzzleLocation = Controlled->Airframe->GetSocketLocation("Gun");
	FRotator MuzzleRotation = Controlled->Airframe->GetSocketRotation("Gun");
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Controlled;
	SpawnParams.Instigator = Controlled->GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AAircraftBullet* SpawnBullet = GetWorld()->SpawnActor<AAircraftBullet>(Bullet, MuzzleLocation, MuzzleRotation, SpawnParams);
	if (!SpawnBullet) return;

	SpawnBullet->FireInDirection(MuzzleRotation.Vector());
}

//Create and Replace Missile in Array
void UWeaponSystemComponent::ReEquip(FCooldownWeapon& Replace)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Controlled;

	Replace.WeaponInstance = GetWorld()->SpawnActor<ABaseWeapon>(Replace.WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

	if (!Replace.WeaponInstance) return;

	FAttachmentTransformRules AttachRules(EAttachmentRule::KeepRelative, true);
	Replace.WeaponInstance->AttachToComponent(Controlled->Airframe, AttachRules, Replace.SocketName);

	FTransform RelativeTransform;
	// TODO: Update pylon to include socket for weapon, current transform is temporary
	RelativeTransform.SetLocation(FVector(0, -175.f, -40.f));
	RelativeTransform.SetRotation(FRotator(0.f, -90.f, 0.f).Quaternion());
	Replace.WeaponInstance->GetRootComponent()->SetRelativeTransform(RelativeTransform);
}

void UWeaponSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

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
	UpdateLockedOn(DeltaTime, Controlled->Tracking);

	FVector Start = Controlled->GetActorLocation();
	FVector Direction = Controlled->GetActorForwardVector();
	float Length = 10000.f;
	float AngleRad = FMath::DegreesToRadians(30.f);

	DrawDebugCone(
		GetWorld(),
		Start,
		Direction,
		Length, 
		AngleRad, 
		AngleRad, 
		12,
		FColor::Red,
		false,
		-1.f,
		0,
		1.f
	);
}

void UWeaponSystemComponent::EquipWeapons()
{
	for (const TPair<FName, TSubclassOf<ABaseWeapon>>&Pair : Loadout)
	{
		// TODO: Introduce socket on Pylon and attach to that, remove transform
		FTransform SocketTransform = Controlled->Airframe->GetSocketTransform(Pair.Key);
		FRotator RotationOffset = FRotator(0.f, -90.f, 0.f);
		FVector PosOffset = FVector(-175, 0, -40.f);
		SocketTransform.ConcatenateRotation(RotationOffset.Quaternion());
		SocketTransform.AddToTranslation(PosOffset);

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = Controlled;
		ABaseWeapon* SpawnIn = GetWorld()->SpawnActor<ABaseWeapon>(Pair.Value, SocketTransform, SpawnParams);
		if (!SpawnIn) continue;
		SpawnIn->AttachToComponent(Controlled->Airframe, FAttachmentTransformRules::KeepWorldTransform, Pair.Key);

		if (!CurrentWeapon) {
			CurrentWeapon = SpawnIn;
		}

		FCooldownWeapon tempCool;
		tempCool.WeaponClass = Pair.Value;
		tempCool.WeaponInstance = SpawnIn;
		tempCool.bCanFire = true;
		tempCool.cooldownTime = SpawnIn->cooldownTime;
		tempCool.time = 0;
		tempCool.SocketName = Pair.Key;
		AvailableWeapons.Add(tempCool);
	}
}

void UWeaponSystemComponent::FireWeaponSelected(int WeaponIndex, AActor* Target, float Speed)
{
	if (!AvailableWeapons.IsValidIndex(WeaponIndex - 1)) return;

	FCooldownWeapon& Weapon = AvailableWeapons[WeaponIndex - 1];

	if (!Weapon.WeaponInstance || !Weapon.CanFire()) return;

	if (Target && bLocked)
	{
		Weapon.WeaponInstance->FireTracking(Speed, Target);
	}
	else
	{
		Weapon.WeaponInstance->FireStatic(Speed);
	}
	Weapon.StartCooldown();
}

void UWeaponSystemComponent::SelectWeapon(int WeaponIndex)
{
	if (AvailableWeapons.IsValidIndex(WeaponIndex - 1))
	{
		CurrentWeapon = AvailableWeapons[WeaponIndex - 1].WeaponInstance;
	}
}
// TODO: Make the cone angle weapon specific (or maybe general)
float CONE_ANGLE = 30.f;

void UWeaponSystemComponent::UpdateLockedOn(float DeltaSeconds, AActor* Target) 
{
	if (!CurrentWeapon || !CurrentWeapon->canLock) return;
	if (!Target) {
		bLocked = false;
		LockTime = 0.f;
		return;
	}

	FVector ToTarget = Target->GetActorLocation() - Controlled->GetActorLocation();
	float Distance = ToTarget.Size();
	ToTarget.Normalize();
	if (Distance > CurrentWeapon->range) 
	{
		bLocked = false;
		LockTime = 0.f;
		return;
	}

	float Dot = FVector::DotProduct(Controlled->Airframe->GetForwardVector(), ToTarget);
	bool bInCone = Dot > FMath::Cos(FMath::DegreesToRadians(CONE_ANGLE));
	if (bInCone)
	{
		//if (bLocked) return;
		LockTime += DeltaSeconds;
		bLocked = LockTime >= 1;
	}
	else
	{
		bLocked = false;
		LockTime = 0.f;
	}

}