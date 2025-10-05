// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Missile!"));
#include "Weapons/Missiles/BaseIRMissile.h"
#include "NiagaraFunctionLibrary.h"

ABaseIRMissile::ABaseIRMissile()
{
	PrimaryActorTick.bCanEverTick = true;
	timeTilDelt = 0;
	isAir = false;
	canLock = true;

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Missile Collision"));
	RootComponent = Collision;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Missile"));
	WeaponMesh->SetupAttachment(RootComponent);
}

void ABaseIRMissile::BeginPlay()
{
	Super::BeginPlay();
	if (!MissileStats) return;
	WeaponName = MissileStats->InGameName;
	missileAcceleration = MissileStats->Acceleration;
	missileMaxSpeed = MissileStats->MaxSpeed;
	cooldownTime = MissileStats->Cooldown;
	range = MissileStats->LockOnRange;
	turnRate = MissileStats->TurnRate;
}

void ABaseIRMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!isAir) return;

	// VFX
	if (SmokeTrail) 
	{
		SmokeTrail->SetWorldLocation(WeaponMesh->GetSocketLocation(TEXT("ExhaustSocket")));
		SmokeTrail->SetWorldRotation(WeaponMesh->GetSocketRotation(TEXT("ExhaustSocket")));
	}

	// Missile is Fired

	missileVelocity += missileAcceleration * DeltaTime;
	missileVelocity = FMath::Clamp(missileVelocity, 0.f, missileMaxSpeed);

	// Movement Forward

	FVector DeltaMove = GetActorForwardVector() * missileVelocity * DeltaTime;
	AddActorWorldOffset(DeltaMove, true);

	bool destroyNeeded = false;

	if (Tracking) 
	{
		float pitchRad = calculatePitchAngle();
		float yawRad = calculateYawAngle();
		FRotator DeltaRot(pitchRad, yawRad, 0);
		AddActorLocalRotation(DeltaRot);
		FVector Distance = (Tracking->GetActorLocation() - this->GetActorLocation());
		destroyNeeded = Distance.Size() <= 1000.f;
	}

	timeTilDelt += DeltaTime;

	// Missile explodes at range

	if (!(timeTilDelt >= MissileStats->LifeTime) && !destroyNeeded) return;

	if (SmokeTrail) SmokeTrail->Deactivate();

	Destroy();
}

float ABaseIRMissile::calculatePitchAngle() 
{
	FVector MissileLoc = this->GetActorLocation();
	FVector ToTarget = Tracking->GetActorLocation() - MissileLoc;
	if (ToTarget.IsNearlyZero()) return 0.f;
	ToTarget.Normalize();

	FVector Forward = this->GetActorForwardVector();
	FVector Up = this->GetActorUpVector();
	FVector Right = this->GetActorRightVector();

	FVector ToTargetInPitch = ToTarget - FVector::DotProduct(ToTarget, Right) * Right;
	if (ToTargetInPitch.IsNearlyZero()) return 0.f;
	ToTargetInPitch.Normalize();

	float pitchRad = FMath::Atan2(FVector::DotProduct(ToTargetInPitch, Up), FVector::DotProduct(ToTargetInPitch, Forward));
	pitchRad = FMath::Clamp(pitchRad, -turnRate, turnRate);

	return pitchRad;
}

float ABaseIRMissile::calculateYawAngle()
{
	FVector MissileLoc = this->GetActorLocation();
	FVector ToTarget = Tracking->GetActorLocation() - MissileLoc;
	if (ToTarget.IsNearlyZero()) return 0.f;
	ToTarget.Normalize();

	FVector Forward = this->GetActorForwardVector();
	FVector Right = this->GetActorRightVector();
	FVector Up = this->GetActorUpVector();

	FVector FlatToTarget = ToTarget - FVector::DotProduct(ToTarget, Up) * Up;
	if (FlatToTarget.IsNearlyZero()) return 0.f;
	FlatToTarget.Normalize();

	float yawRad = FMath::Atan2(
		FVector::DotProduct(FlatToTarget, Right),
		FVector::DotProduct(FlatToTarget, Forward)
	);

	yawRad = FMath::Clamp(yawRad, -turnRate, turnRate);

	return yawRad;
}

void ABaseIRMissile::FireStatic(float launchSpeed)
{
	LaunchSequence(launchSpeed);
}

void ABaseIRMissile::FireTracking(float launchSpeed, AActor* Target) 
{
	Tracking = Target;
	LaunchSequence(launchSpeed);
}

void ABaseIRMissile::LaunchSequence(float Speed) 
{
	if (GetOwner())
	{
		// ====================================
		// Unbound Missile from Socket
		// ====================================

		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetCollisionObjectType(ECC_PhysicsBody);
		WeaponMesh->SetCollisionResponseToAllChannels(ECR_Block);
		WeaponMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
		missileVelocity = Speed;
		isAir = true;

		SmokeTrail = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			SmokeTrailSystem,
			WeaponMesh->GetSocketLocation(TEXT("ExhaustSocket")),
			WeaponMesh->GetSocketRotation(TEXT("ExhaustSocket")),
			FVector(1.f),
			false,
			true
		);
	}
}

float ABaseIRMissile::ReturnCooldownTime() {return cooldownTime;}