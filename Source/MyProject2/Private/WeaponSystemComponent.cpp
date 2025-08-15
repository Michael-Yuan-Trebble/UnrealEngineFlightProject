// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponSystemComponent.h"
#include "Aircraft/BaseAircraft.h"
#include "Weapons/AircraftBullet.h"

UWeaponSystemComponent::UWeaponSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWeaponSystemComponent::Setup(ABaseAircraft* InBase, UAircraftStats* InStats) {
	Controlled = InBase;
	AirStats = InStats;
}

void UWeaponSystemComponent::FireBullets()
{
	if (!Bullet || !(Controlled->Airframe->DoesSocketExist("Gun"))) return;

	FVector MuzzleLocation = Controlled->Airframe->GetSocketLocation("Gun");
	FRotator MuzzleRotation = Controlled->Airframe->GetSocketRotation("Gun");
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AAircraftBullet* SpawnBullet = GetWorld()->SpawnActor<AAircraftBullet>(Bullet, MuzzleLocation, MuzzleRotation, SpawnParams);
	if (!SpawnBullet) return;

	SpawnBullet->FireInDirection(MuzzleRotation.Vector());
}

//Create and Replace Missile in Array
void UWeaponSystemComponent::ReEquip(FCooldownWeapon* Replace)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Controlled;
	//Replace->Current = GetWorld()->SpawnActor<AR60>(AR60::StaticClass(), Airframe->GetSocketTransform(Replace->SocketName),SpawnParams);
	//Replace->Current->AttachToComponent(Airframe, FAttachmentTransformRules::SnapToTargetIncludingScale, Replace->SocketName);
}

void UWeaponSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UWeaponSystemComponent::EquipWeapons(TArray<TSubclassOf<ABaseWeapon>> WeaponClasses)
{
	for (int i = 0; i < WeaponClasses.Num(); i++)
	{
		if (WeaponClasses[i] == nullptr) continue;
		//Find Socket, attach Weapon to that Socket

		FName SocketName = FName(*FString::Printf(TEXT("Pylon%d"), i + 1));
		if (Controlled->GetMesh()->GetSkeletalMeshAsset()->GetSkeleton()->FindSocket(SocketName))
		{
			FTransform SocketTransform = Controlled->Airframe->GetSocketTransform(SocketName);
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = Controlled;
			/*
			AR60* Weapon = GetWorld()->SpawnActor<AR60>(WeaponClasses[i], SocketTransform, SpawnParams);

			if (!Weapon) continue;
				Weapon->SetActorEnableCollision(false);
				Weapon->AttachToComponent(Airframe, FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);

				FCooldownWeapon tempCool;
				tempCool.Current = Weapon;
				tempCool.bCanFire = true;
				tempCool.cooldownTime = Weapon->ReturnCooldownTime();
				tempCool.time = 0;
				tempCool.SocketName = SocketName;

				AvailableWeapons.Add(tempCool);

				CurrentWeapon = Cast<ABaseIRMissile>(Weapon);*/
		}
	}
}

void UWeaponSystemComponent::FireWeaponNotSelected(int WeaponIndex)
{
	if (AvailableWeapons.IsValidIndex(WeaponIndex - 1))
	{
		if (AvailableWeapons[WeaponIndex - 1].Current && AvailableWeapons[WeaponIndex - 1].CanFire())
		{
			//AvailableWeapons[WeaponIndex - 1].Current->FireStatic(currentSpeed);
			AvailableWeapons[WeaponIndex - 1].StartCooldown();
		}
	}
}

void UWeaponSystemComponent::FireWeaponSelected(int WeaponIndex, AActor* Target)
{
	if (AvailableWeapons.IsValidIndex(WeaponIndex - 1))
	{
		if (AvailableWeapons[WeaponIndex - 1].Current && AvailableWeapons[WeaponIndex - 1].CanFire())
		{
			if (bLocked)
			{
				AvailableWeapons[WeaponIndex - 1].Current->SetActorEnableCollision(true);
				//AvailableWeapons[WeaponIndex - 1].Current->FireTracking(currentSpeed, Target);
				AvailableWeapons[WeaponIndex - 1].StartCooldown();
			}
			else
			{
				FireWeaponNotSelected(WeaponIndex);
			}
		}
	}
}

void UWeaponSystemComponent::SelectWeapon(int WeaponIndex)
{
	if (AvailableWeapons.IsValidIndex(WeaponIndex - 1))
	{
		CurrentWeapon = AvailableWeapons[WeaponIndex - 1].Current;
	}
}

