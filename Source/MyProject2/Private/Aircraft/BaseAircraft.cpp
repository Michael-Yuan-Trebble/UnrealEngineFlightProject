// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Base!"));
#include "Aircraft/BaseAircraft.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "Weapons/Missiles/R60.h"
#include "Structs and Data/CooldownWeapon.h"
#include "Weapons/AircraftBullet.h"
#include "AircraftPlayerController.h"

enum class ThrottleStage 
{
	Slow,
	Normal,
	Afterburner
};

/*
	Different "stages" of throttle
	0-40 = slow down
	41-80 = normal
	81=100 = afterburner
*/

ThrottleStage getThrottleStage(float throttle) 
{
	throttle *= 100.f;
	if (throttle >= 0 && throttle <= 40) return ThrottleStage::Slow;
	else if (throttle <= 80) return ThrottleStage::Normal;
	else return ThrottleStage::Afterburner;
}

//Initialize BaseAircraft
ABaseAircraft::ABaseAircraft()
{
	//Mesh Components
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComponent);
	Airframe = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Airframe"));
	Airframe->SetupAttachment(RootComponent);

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Collision->SetupAttachment(Airframe);

	BodyCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BodyCollision"));
	BodyCollision->SetupAttachment(Collision);

	LeftWingCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftWingCollision"));
	LeftWingCollision->SetupAttachment(Collision);

	RightWingCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightWingCollision"));
	RightWingCollision->SetupAttachment(Collision);

	RudderCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RudderCollision"));
	RudderCollision->SetupAttachment(Collision);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->bDoCollisionTest = false;
	SpringArm->SetupAttachment(Airframe);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	PrimaryActorTick.bCanEverTick = true;
	bLocked = false;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PylonMeshObj(TEXT("/Game/Weapons/Pylons/NATO_Pylon.NATO_Pylon"));
	if (!PylonMeshObj.Succeeded()) return; 
	Pylon = PylonMeshObj.Object;
}

void ABaseAircraft::BeginPlay()
{
	Super::BeginPlay();
}

//On Possession Function
void ABaseAircraft::PossessedBy(AController* NewController) 
{
	Super::PossessedBy(NewController);

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
			if (AvailableWeapons[i].CanFire()) ReEquip(&AvailableWeapons[i]);
		}
	}
	UpdateLockedOn(DeltaTime);

	float ConeLength = 0;

	if (!CurrentWeapon) return; 
	ConeLength = CurrentWeapon->range;

	DrawDebugCone(
		GetWorld(),
		Airframe->GetComponentLocation(),
		Airframe->GetForwardVector(),
		ConeLength,
		FMath::DegreesToRadians(30 * 0.5f),
		FMath::DegreesToRadians(30 * 0.5f),
		12,
		FColor::Red,
		false,
		0.1,
		0
	);

	if (!isFlying && currentSpeed > 10.f) isFlying = true;

	Super::Tick(DeltaTime);
}

void ABaseAircraft::FireBullets() {
	if (!Bullet || !(Airframe->DoesSocketExist("Gun"))) return;

	FVector MuzzleLocation = Airframe->GetSocketLocation("Gun");
	FRotator MuzzleRotation = Airframe->GetSocketRotation("Gun");
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AAircraftBullet* SpawnBullet = GetWorld()->SpawnActor<AAircraftBullet>(Bullet, MuzzleLocation, MuzzleRotation, SpawnParams);
	if (!SpawnBullet) return; 
	
	SpawnBullet->FireInDirection(MuzzleRotation.Vector());
}

void ABaseAircraft::ApplySpeed(float ThrottlePercentage, float DeltaSeconds)
{

	ThrottleStage currentStage = getThrottleStage(ThrottlePercentage);
	switch (currentStage) 
	{
		case ThrottleStage::Slow:
			SlowSpeed(ThrottlePercentage);
			break;
		case ThrottleStage::Normal:
			NormalSpeed(ThrottlePercentage);
			break;
		case ThrottleStage::Afterburner:
			AfterburnerSpeed(ThrottlePercentage);
			break;
	}
	float drag = 15.f / (1.f + FMath::Pow(2.f, -0.1f * (currentSpeed - 650.f)));

	currentSpeed -= drag;
	currentSpeed = FMath::Clamp(currentSpeed, 0, ListedMaximumSpeed);

	float InterpSpeed = FMath::FInterpTo(Velocity.Size(), currentSpeed, DeltaSeconds, 2.f);

	FVector Forward = GetActorForwardVector();

	Velocity = Forward * InterpSpeed;

	AddActorWorldOffset(Velocity * DeltaSeconds);

	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + Forward * 300, FColor::Blue, false, 0.f, 0, 2.f);
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + Airframe->GetForwardVector() * 300, FColor::Green, false, 0.f, 0, 2.f);
}

void ABaseAircraft::SlowSpeed(float ThrottlePercentage) 
{
	float negation = 1 - ThrottlePercentage;
	if (ThrottlePercentage <= 0.1) 
	{
		currentSpeed += -(ListedThrust * negation * 5);
	}
	else 
	{
		currentSpeed += -(ListedThrust * negation * 2);
	}
}

void ABaseAircraft::NormalSpeed(float ThrottlePercentage) 
{
	currentSpeed += ListedThrust * ThrottlePercentage;
}

void ABaseAircraft::AfterburnerSpeed(float ThrottlePercentage) 
{
	if (ThrottlePercentage >= 0.9) 
	{
		currentSpeed += ListedThrust * ThrottlePercentage * 5;
	}
	else 
	{
		currentSpeed += ListedThrust * ThrottlePercentage * 2;
	}
}

void ABaseAircraft::AdjustSpringArm(float DeltaSeconds, float ThrottlePercentage) 
{
	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, springArmLength + (50 * (0.5 - ThrottlePercentage)), DeltaSeconds, 2.f);
}

float ABaseAircraft::DragAOA(float AOA) 
{
	float AOARadians = FMath::DegreesToRadians(AOA);
	float drag = 0.75 * FMath::Pow(AOARadians,2);
	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("%f"), AOARadians));
	return drag;
}

void ABaseAircraft::ApplyRot(float DeltaSeconds)
{
	ApplyYaw(DeltaSeconds);
	ApplyRoll(DeltaSeconds);
	ApplyPitch(DeltaSeconds);

	FQuat DeltaRotation = isFlying ? FQuat(FRotator(NextPitch, NextYaw, NextRoll))
		: FQuat(FRotator(0, NextYaw, 0));

	Airframe->AddLocalRotation(DeltaRotation);

	ReturnAOA(DeltaSeconds);
}

//Calculations
void ABaseAircraft::ApplyPitch(float DeltaSeconds) 
{
	if (FMath::IsNearlyZero(UserPitch, 0.01f)) 
	{ 
		NextPitch = FMath::FInterpTo(NextPitch, 0, DeltaSeconds, 5.f); 
	}
	else 
	{
		NextPitch = FMath::FInterpTo(NextPitch, UserPitch, DeltaSeconds, 8.f);
		NextPitch = FMath::Clamp(NextPitch, -ListedTurnRate, ListedTurnRate);

		if (UserPitch < 0.01) NextPitch *= 0.7f;
	}
}

void ABaseAircraft::ApplyYaw(float DeltaSeconds) 
{
	if (UserYaw == 0) NextYaw = FMath::FInterpTo(NextYaw, 0, DeltaSeconds, 7.f);
	else 
	{
		NextYaw = FMath::FInterpTo(NextYaw, UserYaw, DeltaSeconds, 2.f);
		NextYaw = FMath::Clamp(NextYaw, -ListedRudder, ListedRudder);
	}
}

void ABaseAircraft::ApplyRoll(float DeltaSeconds) 
{
	if (UserRoll == 0) NextRoll = FMath::FInterpTo(NextRoll, 0, DeltaSeconds, 3.5f);
	else
	{
		NextRoll = FMath::FInterpTo(NextRoll, UserRoll, DeltaSeconds, 8.f);
		NextRoll = FMath::Clamp(NextRoll, -RollRate, RollRate);
	}
}
//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("X = %f Y = %f Z = %f"),RelFor.X, RelFor.Y, RelFor.Z));
void ABaseAircraft::ReturnAOA(float DeltaSeconds) 
{
	FQuat CurrentQuat = GetActorQuat();
	FQuat TargetQuat = Airframe->GetComponentQuat();
	float RootTurnSpeed = 2.0f;
	float RootAlpha = FMath::Clamp(RootTurnSpeed * DeltaSeconds, 0.f, 1.f);
	FQuat NewRootQuat = FQuat::Slerp(CurrentQuat, TargetQuat, RootAlpha);
	SetActorRotation(NewRootQuat);

	FQuat AirframeCurrentRelQuat = Airframe->GetRelativeRotation().Quaternion();
	FQuat IdentityQuat = FQuat::Identity;
	float NoseAlpha = FMath::Clamp(AOAReturnSpeed * DeltaSeconds, 0.f, 1.f);
	FQuat NewRelQuat = FQuat::Slerp(AirframeCurrentRelQuat, IdentityQuat, NoseAlpha);
	Airframe->SetRelativeRotation(NewRelQuat.Rotator());
}

//Setting User Input Values
void ABaseAircraft::SetPitch(float PitchValue)
{
	UserPitch = PitchValue;
}

void ABaseAircraft::SetYaw(float YawValue)
{
	UserYaw = YawValue;
}

void ABaseAircraft::SetRoll(float RollValue)
{
	UserRoll = RollValue;
}

//Create and Replace Missile in Array
void ABaseAircraft::ReEquip(FCooldownWeapon* Replace) 
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	Replace->Current = GetWorld()->SpawnActor<AR60>(AR60::StaticClass(), Airframe->GetSocketTransform(Replace->SocketName),SpawnParams);
	Replace->Current->AttachToComponent(Airframe, FAttachmentTransformRules::SnapToTargetIncludingScale, Replace->SocketName);
}

void ABaseAircraft::EquipWeapons(const TArray<TSubclassOf<ABaseIRMissile>>& WeaponClasses) 
{
	for (int i = 0; i < WeaponClasses.Num(); i++) 
	{
		if (WeaponClasses[i] == nullptr) continue;
			//Find Socket, attach Weapon to that Socket

		FName SocketName = FName(*FString::Printf(TEXT("Pylon%d"), i + 1));
		if (GetMesh()->GetSkeletalMeshAsset()->GetSkeleton()->FindSocket(SocketName))
		{
			FTransform SocketTransform = Airframe->GetSocketTransform(SocketName);
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
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

				CurrentWeapon = Cast<ABaseIRMissile>(Weapon);
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
			if (bLocked) 
			{
				AvailableWeapons[WeaponIndex - 1].Current->SetActorEnableCollision(true);
				AvailableWeapons[WeaponIndex - 1].Current->FireTracking(currentSpeed, Target);
				AvailableWeapons[WeaponIndex - 1].StartCooldown();
			}
			else 
			{
				FireWeaponNotSelected(WeaponIndex);
			}
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
	for (int i = 1; i <= NumPylons; i++) 
	{
		UStaticMeshComponent* TempPylon = NewObject<UStaticMeshComponent>(this);
		FString SocketName = FString::Printf(TEXT("Pylon%d"), i);
		if (TempPylon && Pylon) 
		{
			TempPylon->SetupAttachment(Airframe, FName(*SocketName));
			TempPylon->RegisterComponent();
			TempPylon->SetStaticMesh(Pylon);
			TempPylon->SetRelativeLocation(FVector(0,-100,-40));
		}
		PylonSockets.Add(FName(*FString::Printf(TEXT("Pylon%d"), i)));
	}
}

void ABaseAircraft::UpdateLockedOn(float DeltaSeconds) 
{
	if (!Tracking) return;

	FVector ToTarget = Tracking->GetActorLocation() - GetActorLocation();
	float Distance = ToTarget.Size();
	ToTarget.Normalize();

	float Dot = FVector::DotProduct(Airframe->GetForwardVector(), ToTarget);
	bool bInRange = false;
	if (CurrentWeapon) bInRange = Distance <= CurrentWeapon->range;
	bool bInCone = Dot > FMath::Cos(FMath::DegreesToRadians(30.f));
	if (bInRange && bInCone) 
	{
		LockTime += DeltaSeconds;
		bLocked = LockTime >= 1 ? true : false;
	}
	else 
	{
		bLocked = false;
		LockTime = 0.f;
	} 
}


//Return Functions

FVector ABaseAircraft::GetTargetLocation() const 
{
	return this->GetActorLocation();
}

bool ABaseAircraft::IsLockable() const 
{
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