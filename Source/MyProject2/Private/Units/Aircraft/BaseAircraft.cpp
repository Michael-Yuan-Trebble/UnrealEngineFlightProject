// Fill out your copyright notice in the Description page of Project Settings.

#include "Units/Aircraft/BaseAircraft.h"
#include "Subsystem/AircraftRegistry.h"
#include "Units/Components/Aircraft/FlightComponent.h"
#include "Units/Components/Aircraft/WeaponSystemComponent.h"
#include "Units/Components/Aircraft/AircraftVisualComponent.h"
#include "Units/Components/Aircraft/RadarComponent.h"
#include "Units/Components/Aircraft/SpecialSystemComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Structs and Data/Aircraft Data/AircraftAudioData.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Units/Components/Aircraft/AircraftAudioComponent.h"
#include "NiagaraSystem.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Debug/DebugHelper.h"

ABaseAircraft::ABaseAircraft()
{
	Airframe = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Airframe"));
	Airframe->SetupAttachment(UnitRoot);
	Airframe->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	LandingGear = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Landing Gear"));
	LandingGear->SetupAttachment(Airframe);
	LandingGear->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BodyCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BodyCollision"));
	BodyCollision->SetupAttachment(Airframe);
	BodyCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	LeftWingCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftWingCollision"));
	LeftWingCollision->SetupAttachment(Airframe);
	LeftWingCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RightWingCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightWingCollision"));
	RightWingCollision->SetupAttachment(Airframe);
	RightWingCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RudderCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RudderCollision"));
	RudderCollision->SetupAttachment(Airframe);
	RudderCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	LandingGearCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LandingGearCollision"));
	LandingGearCollision->SetupAttachment(Airframe);
	LandingGearCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Constraint->SetConstrainedComponents(LandingGearCollision, NAME_None, UnitRoot, NAME_None);

	FlightComponent = CreateDefaultSubobject<UFlightComponent>(TEXT("FlightComponent"));
	RadarComponent = CreateDefaultSubobject<URadarComponent>(TEXT("Radar"));
	WeaponComponent = CreateDefaultSubobject<UWeaponSystemComponent>(TEXT("WeaponComponent"));
	SpecialComp = CreateDefaultSubobject<USpecialSystemComponent>(TEXT("SpecialComponent"));

	PrimaryActorTick.bCanEverTick = true;

	UnitType = ETargetType::Air;
	health = 100;
}

void ABaseAircraft::BeginPlay()
{
	Super::BeginPlay();

	GetBulletStats();

	if (IsValid(VisualCompClass)) 
	{
		VisualComp = NewObject<UAircraftVisualComponent>(this, VisualCompClass);
		if (IsValid(VisualComp)) 
		{
			VisualComp->RegisterComponent();
			VisualComp->SetMesh(Airframe);
		}
	}

	if (!IsValid(Airframe) || !IsValid(RadarComponent) || !IsValid(FlightComponent) || !IsValid(UnitRoot)) return;

	UAircraftStats* LoadedStats = AirStats.LoadSynchronous();
	if (!IsValid(LoadedStats)) return;

	UnitName = LoadedStats->AircraftName;

	OriginalCollOffset = UnitRoot->GetRelativeLocation();
	OriginalExtent = UnitRoot->GetUnscaledBoxExtent();

	RadarComponent->Setup(this);
	FlightComponent->SetLanded(bLanded);
	FlightComponent->Setup(this,LoadedStats);
	FlightComponent->SetDropSpeed(DropSpeed);
	FlightComponent->SetStallSpeed(StallSpeed);
	WeaponComponent->Setup(this, LoadedStats);

	if (UAircraftAudioData* LoadedAudio = LoadedStats->AudioData.LoadSynchronous()) 
		AudioComp->SetAudio(LoadedAudio);

	if (IsValid(AfterburnerSystem))
	{
		for (int i = 0; i < NumOfAfterburners; i++)
		{
			FName SocketName = FName(*FString::Printf(TEXT("AfterburnerSocket%d"), i));
			if (!Airframe->DoesSocketExist(SocketName)) continue;

			UNiagaraComponent* tempAfterburner = UNiagaraFunctionLibrary::SpawnSystemAttached(
				AfterburnerSystem,
				Airframe,
				SocketName,
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				EAttachLocation::SnapToTarget,
				false
			);
			if (IsValid(tempAfterburner))
			{
				tempAfterburner->Deactivate();
				AllAfterburners.Add(tempAfterburner);
			}
		}
	}

	if (IsValid(WingVortexSystem))
	{
		for (int i = 0; i < NumOfVortices; i++)
		{
			FName SocketName = FName(*FString::Printf(TEXT("WingVortexSocket%d"), i));
			if (!Airframe->DoesSocketExist(SocketName)) continue;

			UNiagaraComponent* tempVortex = UNiagaraFunctionLibrary::SpawnSystemAttached(
				WingVortexSystem,
				Airframe,
				SocketName,
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				EAttachLocation::SnapToTarget,
				false
			);
			if (IsValid(tempVortex))
			{
				tempVortex->Deactivate();
				AllVortices.Add(tempVortex);
			}
		}
	}

	FlightComponent->OnAfterburnerEngaged.AddDynamic(this, &ABaseAircraft::HandleAfterburnerFX);
	FlightComponent->OnVortexActivate.AddDynamic(this, &ABaseAircraft::HandleVortexFX);

	if (IsValid(LandingGearCollision)) {
	}
}

void ABaseAircraft::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsValid(VisualComp) && IsValid(FlightComponent))
	{
		const FRotator& Rotation = FlightComponent->GetUserRotation();
		VisualComp->SetPitch(Rotation.Pitch);
		VisualComp->SetYaw(Rotation.Yaw);
		VisualComp->SetRoll(Rotation.Roll);
		VisualComp->SetThrust(FlightComponent->GetThrust());
	}
}

void ABaseAircraft::FireWeaponSelected() { if (IsValid(WeaponComponent)) WeaponComponent->FireWeaponSelected(WeaponComponent->GetWeapon()->GetClass(), Tracked, FlightComponent->GetSpeed()); }

void ABaseAircraft::HandleAfterburnerFX(bool isActive) 
{
	for (UNiagaraComponent* FX : AllAfterburners) 
	{
		if (IsValid(FX)) 
		{
			if (isActive) FX->Activate();
			else FX->Deactivate();
		}
	}
}

void ABaseAircraft::HandleVortexFX(bool isActive) 
{
	for (UNiagaraComponent* FX : AllVortices) {
		if (IsValid(FX)) {
			if (isActive) FX->Activate();
			else FX->Deactivate();
		}
	}
}

void ABaseAircraft::DisableAllMainWingVapors() 
{
	for (UStaticMeshComponent* Mesh : AllMainWingVapors) 
	{
		if (IsValid(Mesh)) Mesh->SetVisibility(false);
	}
}

void ABaseAircraft::EnableAllMainWingVapors() {

}

void ABaseAircraft::HandleLOD(FVector CameraLoc) 
{
	// Setting distance to KM
	if (!IsValid(Airframe)) return;
	float Distance = FVector::Dist(CameraLoc, GetActorLocation()) * 0.00001;
	if (Distance >= 5 && bIsVisible) 
	{
		Airframe->SetVisibility(false, true);
		bIsVisible = false;
	}
	else if (!bIsVisible)
	{
		Airframe->SetVisibility(true, true);
		bIsVisible = true;
	}
}

void ABaseAircraft::ActivateSpecial() {
	if (IsValid(SpecialComp)) SpecialComp->ActivateSpecial(this);
	if (IsValid(VisualComp) && VisualComp->IsCountermeasures()) VisualComp->ActivateFlares();
}

void ABaseAircraft::OnCountermeasureDeployed_Implementation() 
{
	for (auto& Missile : IncomingMissiles) 
	{
		if (Missile.IsValid()) 
		{
			Missile->NotifyCountermeasure();
		}
	}
}

void ABaseAircraft::SetLandingGearVisiblility(bool b)
{
	// TODO: For now its hardcoded for testing, but later change it so that the gamemode dictates if landing gear is present
	if (LandingGear) LandingGear->SetVisibility(b);

	// TODO: Eventually have this collision box work, however it doesn't instantly kill the player upon reaching designated ground
	if (LandingGearCollision) LandingGearCollision->SetCollisionEnabled(b ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
};

void ABaseAircraft::Crash() {
	bAlive = true;
}

void ABaseAircraft::SetThrust(const float thrust) { if (IsValid(FlightComponent)) FlightComponent->SetThrust(thrust); }

void ABaseAircraft::SetRoll(const float roll) { if (IsValid(FlightComponent)) FlightComponent->SetRoll(roll); }

void ABaseAircraft::SetPitch(const float pitch) { if (IsValid(FlightComponent)) FlightComponent->SetPitch(pitch); }

void ABaseAircraft::SetRudder(const float rudder) { if (IsValid(FlightComponent)) FlightComponent->SetYaw(rudder); }

void ABaseAircraft::SetFlying(const bool bIsFlying) { if (IsValid(FlightComponent)) FlightComponent->SetFlying(bIsFlying); }

void ABaseAircraft::SetRestrained(bool bIsRestrained) { if (IsValid(FlightComponent)) FlightComponent->SetRestrained(bIsRestrained); }

void ABaseAircraft::SetSpeed(const float speed) { if (IsValid(FlightComponent)) FlightComponent->SetInitialSpeed(speed); }

bool ABaseAircraft::IsLanded() { if (IsValid(FlightComponent)) return FlightComponent->IsLanded(); else return false; }

float ABaseAircraft::GetUnitSpeed() { if (IsValid(FlightComponent)) return FlightComponent->GetUnitSpeed(); else return 0.f; }

float ABaseAircraft::GetKMHSpeed() { if (IsValid(FlightComponent)) return FlightComponent->GetKMHSpeed(); else return 0.f; }

void ABaseAircraft::SetWeapons(const TMap<FName, TSubclassOf<ABaseWeapon>> In) { if (IsValid(WeaponComponent)) WeaponComponent->SetWeapons(In); }

void ABaseAircraft::SetSpecial(const TSubclassOf<UBaseSpecial> In) { if (IsValid(SpecialComp)) SpecialComp->SetSpecial(In); }

void ABaseAircraft::SetFlightMode(const EFlightMode FlightMode)
{ 
	if (IsValid(FlightComponent)) FlightComponent->SetFlightMode(FlightMode); 
	SetLandingGearVisiblility(FlightMode != EFlightMode::Flight);
}

const FAircraftAnimationValues& ABaseAircraft::GetAircraftAnimationValues() {
	if (IsValid(VisualComp)) return VisualComp->GetAircraftAnimationValues();
	return DefaultAnimVal;
}

UBulletStats* ABaseAircraft::GetBulletStats() {
	if (!IsValid(CachedBulletStats))
	{
		if (UAircraftStats* Stats = AirStats.LoadSynchronous())
		{
			CachedBulletStats = Stats->BulletStats.LoadSynchronous();
		}
	}

	return CachedBulletStats;
}

EThrottleStage ABaseAircraft::GetThrottleStage() const { if (IsValid(FlightComponent)) return FlightComponent->ReturnThrottleStage(); else return EThrottleStage::Slow; }

void ABaseAircraft::ApplySpeed(const float Speed, const float D) { if (IsValid(FlightComponent)) FlightComponent->AddSpeed(Speed, D); }

float ABaseAircraft::GetGForce() {
	if (IsValid(FlightComponent)) return FlightComponent->GetGForce();
	return 0.f;
}

float ABaseAircraft::GetMaxWeaponCount() {
	if (IsValid(WeaponComponent)) return WeaponComponent->GetMaxWeaponCount();
	return 0.f;
}

float ABaseAircraft::GetCurrentWeaponCount() {
	if (IsValid(WeaponComponent)) return WeaponComponent->GetCurrentWeaponCount();
	return 0.f;
}

void ABaseAircraft::SwitchWeapon(const TSubclassOf<ABaseWeapon> InWeapon)
{
	if (WeaponComponent) WeaponComponent->SearchAndEquipWeapon(InWeapon);
}