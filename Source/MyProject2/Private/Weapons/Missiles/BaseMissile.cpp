// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Missile!"));
#include "Weapons/Missiles/BaseMissile.h"
#include "Units/Aircraft/BaseAircraft.h"
#include "Subsystem/MissileManagerSubsystem.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "Interfaces/ApproachingMissileInterface.h"

ABaseMissile::ABaseMissile() 
{
	PrimaryActorTick.bCanEverTick = true;
	canLock = true;
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Missile Collision"));
	RootComponent = Collision;

	Collision->SetCollisionProfileName(TEXT("Projectile"));
	Collision->SetNotifyRigidBodyCollision(true);
	Collision->SetGenerateOverlapEvents(false);

	Collision->OnComponentBeginOverlap.AddDynamic(this, &ABaseMissile::OnOverlapBegin);
	Collision->OnComponentHit.AddDynamic(this, &ABaseMissile::OnHit);

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

void ABaseMissile::LaunchSequence(const float speed)
{
}

void ABaseMissile::activateSmoke() 
{
	if (!IsValid(WeaponMesh) || !WeaponMesh->DoesSocketExist(TEXT("ExhaustSocket"))) return;

	if (SmokeTrail.IsValid()|| MissileRocket.IsValid()) return;

	UNiagaraSystem* LoadedSmoke = SmokeTrailSystem.LoadSynchronous();
	if (!IsValid(LoadedSmoke)) return;

	UNiagaraSystem* LoadedRocket = MissileRocketSystem.LoadSynchronous();
	if (!IsValid(LoadedRocket)) return;

	SmokeTrail = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		LoadedSmoke,
		WeaponMesh->GetSocketLocation(TEXT("ExhaustSocket")),
		WeaponMesh->GetSocketRotation(TEXT("ExhaustSocket")),
		FVector(1.f),
		true,
		true
	);

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
	ToTarget.Normalize();
	float Dot = FVector::DotProduct(GetActorForwardVector(), ToTarget);
	if (Dot < 0.f) {
		return true;
	}
	return false;
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
			ProjectileMovement->Velocity = GetActorForwardVector() * missileAcceleration;
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