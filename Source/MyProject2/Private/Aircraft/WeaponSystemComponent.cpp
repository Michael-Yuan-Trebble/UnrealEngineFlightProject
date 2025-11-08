// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Weapon Component!"));
#include "Aircraft/WeaponSystemComponent.h"
#include "Aircraft/BaseAircraft.h"
#include "UI/PlayerHUD.h"
#include "Weapons/AircraftBullet.h"
#include "Kismet/GameplayStatics.h"
#include "Structs and Data/LockableTarget.h"
#include "AircraftPlayerController.h"
#include "DrawDebugHelpers.h"

UWeaponSystemComponent::UWeaponSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWeaponSystemComponent::Setup(ABaseAircraft* InBase, UAircraftStats* InStats) 
{
	Controlled = InBase;
	AirStats = InStats;
	PC = Cast<AAircraftPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PC) HUD = Cast<APlayerHUD>(PC->GetHUD());
}

void UWeaponSystemComponent::FireBullets()
{
	//Spawns a bullet actor whilst firing at socket
	if (!Controlled || !Controlled->BulletStats->BulletClass || !(Controlled->Airframe->DoesSocketExist("Gun"))) return;
	FVector MuzzleLocation = Controlled->Airframe->GetSocketLocation("Gun");
	FRotator MuzzleRotation = Controlled->Airframe->GetSocketRotation("Gun");
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Controlled;
	SpawnParams.Instigator = Controlled->GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AAircraftBullet* SpawnBullet = GetWorld()->SpawnActor<AAircraftBullet>(Controlled->BulletStats->BulletClass, MuzzleLocation, MuzzleRotation, SpawnParams);
	if (!SpawnBullet) return;

	SpawnBullet->FireInDirection(MuzzleRotation.Vector());
}

void UWeaponSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!Controlled || !AirStats) return;

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
}

void UWeaponSystemComponent::SetWeapons(TMap<FName, TSubclassOf<ABaseWeapon>> In) 
{
	if (!AirStats || !Controlled || !Controlled->Airframe) return;
	Loadout = In;
	AddPylons();
	EquipWeapons();
}

void UWeaponSystemComponent::AddPylons() 
{
	for (int i = 0; i < AirStats->NumOfPylons; i++)
	{
		UStaticMeshComponent* TempPylon = NewObject<UStaticMeshComponent>(this);
		FName SocketName = FName(*FString::Printf(TEXT("Pylon%d"), i));
		if (TempPylon && AirStats->Pylon)
		{
			TempPylon->SetupAttachment(Controlled->Airframe, SocketName);
			TempPylon->RegisterComponent();
			TempPylon->SetStaticMesh(AirStats->Pylon);
			TempPylon->SetRelativeLocation(FVector(0, 0, -20));
			PylonSockets.Add(SocketName, TempPylon);
		}
	}
}

void UWeaponSystemComponent::EquipWeapons()
{
	for (const TPair<FName, TSubclassOf<ABaseWeapon>>&Pair : Loadout)
	{
		UStaticMeshComponent* PylonComp = PylonSockets.FindRef(Pair.Key);
		if (!PylonComp)  continue;
		FTransform SocketTransform = PylonComp->GetSocketTransform(FName("Socket"));
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = Controlled;
		ABaseWeapon* SpawnIn = GetWorld()->SpawnActor<ABaseWeapon>(Pair.Value, SocketTransform, SpawnParams);
		if (!SpawnIn) continue;
		SpawnIn->Collision->SetSimulatePhysics(false);
		SpawnIn->AttachToComponent(PylonComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Socket"));

		FCooldownWeapon tempCool;
		tempCool.WeaponClass = Pair.Value;
		tempCool.WeaponInstance = SpawnIn;
		tempCool.bCanFire = true;
		tempCool.cooldownTime = SpawnIn->cooldownTime;
		tempCool.time = 0;
		tempCool.SocketName = Pair.Key;
		AvailableWeapons.Add(tempCool);
	}
	BuildWeaponGroups();
}

//Create and Replace Missile in Array
void UWeaponSystemComponent::ReEquip(FCooldownWeapon& Replace)
{
	UStaticMeshComponent* PylonComp = PylonSockets.FindRef(Replace.SocketName);
	if (!PylonComp) return;
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Controlled;
	Replace.WeaponInstance = GetWorld()->SpawnActor<ABaseWeapon>(Replace.WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	if (!Replace.WeaponInstance) return;
	FTransform SocketTransform = PylonComp->GetSocketTransform(FName("Socket"));
	Replace.WeaponInstance->AttachToComponent(PylonComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Socket"));
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
			EquippedWeaponNames.Add(CW.WeaponInstance->WeaponName);
		}
		WeaponGroups[WeaponClass].Add(&CW);
	}
	if (WeaponGroups.Num() > 0) SelectWeapon(0);
}

void UWeaponSystemComponent::FireWeaponSelected(TSubclassOf<ABaseWeapon> WeaponClass, AActor* Target, float Speed)
{
	if (!WeaponGroups.Contains(WeaponClass)) return;
	for (FCooldownWeapon* Weapon : WeaponGroups[WeaponClass])
	{
		if (!Weapon->WeaponInstance || !Weapon->CanFire()) continue;

		if (Target && bLocked)
		{
			Weapon->WeaponInstance->FireTracking(Speed, Target);
		}
		else 
		{
			Weapon->WeaponInstance->FireStatic(Speed);
		}
		Weapon->StartCooldown();
		GetCount();
		return;
	}
}

void UWeaponSystemComponent::SelectWeapon(int WeaponIndex)
{
	TArray<TSubclassOf<ABaseWeapon>> Keys;
	WeaponGroups.GetKeys(Keys);

	if (!Keys.IsValidIndex(WeaponIndex)) return;

	TSubclassOf<ABaseWeapon> SelectedClass = Keys[WeaponIndex];

	if (PC) PC->CurrentWeaponClass = SelectedClass;

	const TArray<FCooldownWeapon*>* WeaponArray = WeaponGroups.Find(SelectedClass);
	if (!WeaponArray || WeaponArray->Num() == 0) return;

	CurrentWeapon = (*WeaponArray)[0]->WeaponInstance;

	GetCount();
}

void UWeaponSystemComponent::GetCount() 
{
	if (!CurrentWeapon) return;

	MaxWeaponCountSelected = 0;
	CurrentWeaponCount = 0;

	TSubclassOf<ABaseWeapon> CurrentClass = CurrentWeapon->GetClass();

	const TArray<FCooldownWeapon*>* WeaponArray = WeaponGroups.Find(CurrentClass);
	if (!WeaponArray) return;

	MaxWeaponCountSelected = WeaponArray->Num();

	for (const FCooldownWeapon* Weapon : *WeaponArray)
	{
		if (Weapon && Weapon->WeaponInstance && Weapon->bCanFire)
		{
			CurrentWeaponCount++;
		}
	}
	OnWeaponCountUpdated.Broadcast(CurrentWeapon->WeaponName, CurrentWeaponCount, MaxWeaponCountSelected);
}

void UWeaponSystemComponent::UpdateLockedOn(float DeltaSeconds, AActor* Target) 
{
	if (!CurrentWeapon || !CurrentWeapon->canLock || !IsValid(Target)) return;

	if (!Target->Implements<ULockableTarget>()) return; // Breakpoint error

	ILockableTarget* LockTarget = Cast<ILockableTarget>(Target);
	if (!LockTarget) return;

	ETargetType TargetType = LockTarget->GetTargetType();
	

	if (!CurrentWeapon->CanLockTarget(TargetType)) return;

	if (!Target) 
	{
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
		HUD->UpdateLocked(bLocked);
		LockTime = 0.f;
		return;
	}

	float Dot = FVector::DotProduct(Controlled->Airframe->GetForwardVector(), ToTarget);
	bool bInCone = Dot > FMath::Cos(FMath::DegreesToRadians(CONE_ANGLE));
	if (bInCone)
	{
		LockTime += DeltaSeconds;

		// TODO: Make it variable
		bLocked = LockTime >= 1;
	}
	else
	{
		bLocked = false;
		LockTime = 0.f;
	}
	HUD->UpdateLocked(bLocked);
}