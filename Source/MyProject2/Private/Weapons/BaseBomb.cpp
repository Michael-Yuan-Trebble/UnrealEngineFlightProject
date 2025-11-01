// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Bomb!"));
#include "Weapons/BaseBomb.h"
#include "Aircraft/BaseAircraft.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

ABaseBomb::ABaseBomb() 
{
	PrimaryActorTick.bCanEverTick = true;
	canLock = false;
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Bomb Collision"));
	RootComponent = Collision;

	Collision->SetCollisionProfileName(TEXT("Projectile"));
	Collision->SetNotifyRigidBodyCollision(true);
	Collision->SetGenerateOverlapEvents(false);

	Collision->OnComponentBeginOverlap.AddDynamic(this, &ABaseBomb::OnOverlapBegin);
	Collision->OnComponentEndOverlap.AddDynamic(this, &ABaseBomb::OnEndOverlap);
	Collision->OnComponentHit.AddDynamic(this, &ABaseBomb::OnHit);

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bomb"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetupAttachment(RootComponent);
}

void ABaseBomb::BeginPlay() 
{
	Super::BeginPlay();
	if (GetOwner()) {
		Collision->IgnoreActorWhenMoving(GetOwner(), true);
		Owner = Cast<ABaseAircraft>(GetOwner());
	}
	if (!BombStats) return;
	WeaponName = BombStats->InGameName;
	cooldownTime = BombStats->Cooldown;
}

void ABaseBomb::Tick(float DeltaSeconds) 
{
	Super::Tick(DeltaSeconds);
	if (!Owner || !Owner->IsValidLowLevelFast()) return;

	if (!isAir) return;
	
	Velocity += FVector(0.f, 0.f, GetWorld()->GetGravityZ()) * DeltaSeconds;
	Velocity.Z = -FMath::Clamp(FMath::Abs(Velocity.Z), 0, BombStats->MaxSpeed);
	AddActorWorldOffset(Velocity, true);
}

void ABaseBomb::FireStatic(float launchSpeed)
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	isAir = true;

	// Currently to force it to be the same speed as the aircraft, try and find a work around later
	Velocity = GetActorForwardVector() * (launchSpeed / 75);
}

void ABaseBomb::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	CheckAndDelete();
}

void ABaseBomb::OnHit(UPrimitiveComponent* HitComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit) 
{
	CheckAndDelete();
}

void ABaseBomb::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	CheckAndDelete();
}

bool bDestroyed = false;

void ABaseBomb::CheckAndDelete() 
{
	//print(text)
	TArray<AActor*> OverlappedActors;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	bool bHit = UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		GetActorLocation(),
		BombStats->BlastRadius,
		ObjectTypes,
		AActor::StaticClass(),
		ActorsToIgnore,
		OverlappedActors
	);

	if (bHit)
	{
		for (AActor* Actor : OverlappedActors) 
		{
			if (Actor->Implements<UTeamInterface>()) 
			{
				EFaction OtherFaction = Owner->Faction;
				OtherFaction = ITeamInterface::Execute_GetFaction(Actor);
				if (OtherFaction == Owner->Faction) continue;
			}

			if (Actor->Implements<UDamageableInterface>()) 
			{
				IDamageableInterface::Execute_OnDamage(Actor, this, BombStats->Damage);
			}
		}
	}

	if (bDestroyed) return;
	bDestroyed = true;
	Destroy();
}