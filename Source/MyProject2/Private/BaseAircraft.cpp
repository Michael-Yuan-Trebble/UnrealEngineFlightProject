// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Base!"));
#include "BaseAircraft.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "R60.h"
#include "CooldownWeapon.h"
#include "AircraftPlayerController.h"

//Initialize BaseAircraft
ABaseAircraft::ABaseAircraft()
{

	//Mesh Components
	Airframe = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Airframe"));
	RootComponent = Airframe;
	//Airframe->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//Airframe->SetSimulatePhysics(false);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->bDoCollisionTest = false;
	SpringArm->SetupAttachment(Airframe);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	PrimaryActorTick.bCanEverTick = true;
}

void ABaseAircraft::BeginPlay()
{
	Super::BeginPlay();
}

//On Possession Function
void ABaseAircraft::PossessedBy(AController* NewController) 
{
	Super::PossessedBy(NewController);

	//Controller Cast
	Controlled = Cast<AAircraftPlayerController>(NewController);
}

void ABaseAircraft::Tick(float DeltaTime)
{
	//Loop Update Cooldown on Missiles
	for (int i = 0; i < AvailableWeapons.Num(); i++) 
	{
		if (AvailableWeapons[i].Current && !AvailableWeapons[i].CanFire()) 
		{
			AvailableWeapons[i].UpdateCooldown(DeltaTime);
			if (AvailableWeapons[i].CanFire()) 
			{
				ReEquip(&AvailableWeapons[i]);
			}
		}
	}
	Super::Tick(DeltaTime);
}

//Create and Replace Missile in Array
void ABaseAircraft::ReEquip(FCooldownWeapon* Replace) 
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	Replace->Current = GetWorld()->SpawnActor<AR60>(AR60::StaticClass(), Airframe->GetSocketTransform(Replace->SocketName),SpawnParams);
	Replace->Current->AttachToComponent(Airframe, FAttachmentTransformRules::SnapToTargetIncludingScale, Replace->SocketName);
}

void ABaseAircraft::EquipWeapons(const TArray<TSubclassOf<ABaseIRMissile>>& WeaponClasses) {
	for (int i = 0; i < WeaponClasses.Num(); i++) 
	{
		if (WeaponClasses[i] != nullptr) 
		{
			//Find Socket, attach Weapon to that Socket

			FName SocketName = FName(*FString::Printf(TEXT("Pylon%d"), i + 1));
			if (GetMesh()->GetSkeletalMeshAsset()->GetSkeleton()->FindSocket(SocketName))
			{
				FTransform SocketTransform = Airframe->GetSocketTransform(SocketName);
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;
				AR60* Weapon = GetWorld()->SpawnActor<AR60>(WeaponClasses[i], SocketTransform, SpawnParams);

				if (Weapon)
				{
					Weapon->AttachToComponent(Airframe, FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);

					FCooldownWeapon tempCool;
					tempCool.Current = Weapon;
					tempCool.bCanFire = true;
					tempCool.cooldownTime = Weapon->ReturnCooldownTime();
					tempCool.time = 0;
					tempCool.SocketName = SocketName;

					AvailableWeapons.Add(tempCool);
				}
			}
		}
	}
}

void ABaseAircraft::FireWeaponNotSelected(int WeaponIndex) 
{
	if (AvailableWeapons.IsValidIndex(WeaponIndex-1))
	{
		if (AvailableWeapons[WeaponIndex - 1].Current && AvailableWeapons[WeaponIndex - 1].CanFire()) 
		{
			AvailableWeapons[WeaponIndex - 1].Current->FireStatic(currentSpeed);
			AvailableWeapons[WeaponIndex - 1].StartCooldown();
		}
	}
}

void ABaseAircraft::FireWeaponSelected(int WeaponIndex, AActor* Target) 
{
	if (AvailableWeapons.IsValidIndex(WeaponIndex - 1))
	{
		if (AvailableWeapons[WeaponIndex - 1].Current && AvailableWeapons[WeaponIndex - 1].CanFire())
		{
			
			AvailableWeapons[WeaponIndex - 1].Current->FireTracking(currentSpeed, Target);
			AvailableWeapons[WeaponIndex - 1].StartCooldown();
		}
	}
}

void ABaseAircraft::SelectWeapon(int WeaponIndex) 
{
	if (AvailableWeapons.IsValidIndex(WeaponIndex-1)) 
	{
		CurrentWeapon = AvailableWeapons[WeaponIndex-1].Current;
	}
}

void ABaseAircraft::AddPylons() 
{
	for (int i = 0; i < NumPylons; i++) 
	{
		PylonSockets.Add(FName(*FString::Printf(TEXT("Pylon%d"), i + 1)));
	}
}


//Return Functions

FVector ABaseAircraft::GetTargetLocation() const {
	return this->GetActorLocation();
}

bool ABaseAircraft::IsLockable() const {
	return isAlive;
}

float ABaseAircraft::ReturnAcceleration() const 
{
	return ListedAcceleration;
}

float ABaseAircraft::ReturnTakeoffSpeed() const 
{
	return TakeoffSpeed;
}

float ABaseAircraft::ReturnRollRate() const 
{
	return RollRate;
}

float ABaseAircraft::ReturnTurnRate() const 
{
	return ListedTurnRate;
}

float ABaseAircraft::ReturnMaxSpeed() const 
{
	return ListedMaximumSpeed;
}

float ABaseAircraft::ReturnThrust() const 
{
	return ListedThrust;
}

float ABaseAircraft::ReturnRudder() const 
{
	return ListedRudder;
}

float ABaseAircraft::ReturnSpringArmLength() const 
{
	return springArmLength;
}

USpringArmComponent* ABaseAircraft::GetSpringArm() const 
{
	return SpringArm;
}

UCameraComponent* ABaseAircraft::GetCamera() const 
{
	return Camera;
}

USkeletalMeshComponent* ABaseAircraft::GetMesh() const 
{
	return Airframe;
}