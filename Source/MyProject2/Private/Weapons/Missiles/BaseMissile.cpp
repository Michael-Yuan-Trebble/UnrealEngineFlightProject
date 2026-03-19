// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/Missiles/BaseMissile.h"
#include "Units/Aircraft/BaseAircraft.h"
#include "Subsystem/MissileManagerSubsystem.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "Interfaces/ApproachingMissileInterface.h"
#include "Debug/DebugHelper.h"

ABaseMissile::ABaseMissile() 
{
	PrimaryActorTick.bCanEverTick = true;
	canLock = true;
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Missile Collision"));
	RootComponent = Collision;

	Collision->SetCollisionProfileName(TEXT("Projectile"));

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 0;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Missile"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetupAttachment(RootComponent);
}

void ABaseMissile::BeginPlay() 
{
	Super::BeginPlay();
	if (GetOwner()) 
	{
		Collision->IgnoreActorWhenMoving(GetOwner(), true);
		AircraftOwner = Cast<ABaseAircraft>(GetOwner());
	}

	Collision->SetGenerateOverlapEvents(true);
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ABaseMissile::OnOverlapBegin);
	Collision->OnComponentHit.AddDynamic(this, &ABaseMissile::OnHit);

	if (UMissileManagerSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UMissileManagerSubsystem>()) 
	{
		Subsystem->RegisterMissile(this);
	}
}

void ABaseMissile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UMissileManagerSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UMissileManagerSubsystem>())
	{
		Subsystem->UnregisterMissile(this);
	}
	Super::EndPlay(EndPlayReason);
}

void ABaseMissile::Tick(float DeltaTime) 
{
	Super::Tick(DeltaTime);

}

void ABaseMissile::TurnTowardTarget(float Delta) {
	if (AActor* Loaded = Tracking.Get()) {
		FVector TargetLocation = Loaded->FindComponentByClass<UMeshComponent>()->GetComponentLocation();

		FVector ToTarget = (TargetLocation - GetActorLocation()).GetSafeNormal();
		FVector CurrentForward = GetActorForwardVector();

		FRotator CurrentRot = CurrentForward.Rotation();
		FRotator TargetRot = ToTarget.Rotation();

		float MaxDelta = InGameStats.TurnRate;

		FVector NewRot = FMath::VInterpNormalRotationTo(CurrentForward, ToTarget, Delta, InGameStats.TurnRate);

		//DEBUG_TIME(100.f, "X: %f Y: %f Z: %f", CurrentRot.Roll, CurrentRot.Pitch, CurrentRot.Yaw);

		SetActorRotation(NewRot.Rotation());
	}
}

void ABaseMissile::LaunchSequence(const float speed)
{
}

void ABaseMissile::activateSmoke() 
{
	if (!IsValid(WeaponMesh) || !WeaponMesh->DoesSocketExist(TEXT("ExhaustSocket"))) return;

	if (SmokeTrail.IsValid()|| MissileRocket.IsValid()) return;

	UNiagaraSystem* LoadedSmoke = SmokeTrailSystem.LoadSynchronous();
	if (!IsValid(LoadedSmoke)) return;

	SmokeTrail = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		LoadedSmoke,
		WeaponMesh->GetSocketLocation(TEXT("ExhaustSocket")),
		WeaponMesh->GetSocketRotation(TEXT("ExhaustSocket")),
		FVector(1.f),
		true,
		true
	);

	UNiagaraSystem* LoadedRocket = MissileRocketSystem.LoadSynchronous();
	if (!IsValid(LoadedRocket)) return;

	MissileRocket = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		LoadedRocket,
		WeaponMesh->GetSocketLocation(TEXT("ExhaustSocket")),
		WeaponMesh->GetSocketRotation(TEXT("ExhaustSocket")),
		FVector(1.f),
		true,
		true
	);

	ProjectileMovement->Velocity = GetActorForwardVector() * ProjectileMovement->InitialSpeed;
}

bool ABaseMissile::CalculateIfOvershoot(FVector ToTarget) {
	bool bOvershot = false;
	if (AActor* Loaded = Tracking.Get()) {
		FVector TargetLocation = Loaded->GetActorLocation();
		float CurrentDistance = FVector::Dist(GetActorLocation(), TargetLocation);

		FVector VelocityDir = ProjectileMovement->Velocity.GetSafeNormal();
		FVector ToTarget = (TargetLocation - GetActorLocation()).GetSafeNormal();

		float Dot = FVector::DotProduct(VelocityDir, ToTarget);

		bOvershot = (Dot < 0.f) && (CurrentDistance > PreviousDistance);

		PreviousDistance = CurrentDistance;
	}
	return bOvershot;
}

void ABaseMissile::ApplyVFXLOD(const FVector& CameraLoc)
{
	if (!SmokeTrail.IsValid() || !MissileRocket.IsValid()) return;
	// Make Distance into KM units
	float Distance = FVector::Dist(CameraLoc, GetActorLocation()) * 0.00001;

	if (Distance > 5.f && bMissileVFXOn) 
	{
		SmokeTrail->Deactivate();
		MissileRocket->Deactivate();
		bMissileVFXOn = false;
	}
	else if (!bMissileVFXOn)
	{
		SmokeTrail->Activate(true);
		MissileRocket->Activate(true);
		bMissileVFXOn = true;
	}
}

void ABaseMissile::NotifyCountermeasure() 
{
	if (AActor* LoadedTracking = Tracking.Get()) {
		if (IsValid(ProjectileMovement))
		{
			if (ABaseAircraft* Aircraft = Cast<ABaseAircraft>(LoadedTracking)) {
				if (Aircraft->Implements<UApproachingMissileInterface>())
				{
					IApproachingMissileInterface::Execute_UnregisterIncomingMissile(Aircraft, this);
				}
			}
			ProjectileMovement->bIsHomingProjectile = false;
			ProjectileMovement->HomingTargetComponent = nullptr;
			ProjectileMovement->Velocity = GetActorForwardVector() * InGameStats.Acceleration;
			Tracking = nullptr;
		}
	}
}

void ABaseMissile::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	CheckAndDelete(OtherActor);
}

void ABaseMissile::OnHit(UPrimitiveComponent* HitComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	CheckAndDelete(OtherActor);
}

void ABaseMissile::CheckAndDelete(AActor* OtherActor)
{

}

void ABaseMissile::DestroyMissile()
{
	if (bDestroyed) return;
	bDestroyed = true;

	if (UNiagaraSystem* LoadedExplosion = ExplosionEffect.LoadSynchronous()) {
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			LoadedExplosion,
			GetActorLocation(),
			GetActorRotation()
		);
	}

	if (SmokeTrail.IsValid()) SmokeTrail->Deactivate();
	if (MissileRocket.IsValid()) MissileRocket->Deactivate();

	if (UMissileManagerSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UMissileManagerSubsystem>())
	{
		Subsystem->UnregisterMissile(this);
	}

	Destroy();
}