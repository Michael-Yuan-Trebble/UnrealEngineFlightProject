// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Base!"));
#include "Units/Aircraft/BaseAircraft.h"
#include "Subsystem/AircraftRegistry.h"
#include "Units/Aircraft/FlightComponent.h"
#include "Units/Aircraft/WeaponSystemComponent.h"
#include "Units/Aircraft/AircraftVisualComponent.h"
#include "Units/Aircraft/RadarComponent.h"
#include "Units/Aircraft/SpecialSystemComponent.h"
#include "Weapons/Missiles/BaseMissile.h"

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

	if (IsValid(VisualCompClass)) 
	{
		VisualComp = NewObject<UAircraftVisualComponent>(this, VisualCompClass);
		if (IsValid(VisualComp)) 
		{
			VisualComp->RegisterComponent();
			VisualComp->SetMesh(Airframe);
		}
	}

	if (!IsValid(Airframe) || !IsValid(RadarComponent) || !IsValid(FlightComponent)) return;

	OriginalCollOffset = UnitRoot->GetRelativeLocation();
	OriginalExtent = UnitRoot->GetUnscaledBoxExtent();

	RadarComponent->Setup(this);
	FlightComponent->SetLanded(bLanded);
	FlightComponent->Setup(this, AirStats);
	FlightComponent->SetDropSpeed(DropSpeed);
	FlightComponent->SetStallSpeed(StallSpeed);
	WeaponComponent->Setup(this, AirStats);

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

void ABaseAircraft::PossessedBy(AController* NewController) 
{
	Super::PossessedBy(NewController);
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

void ABaseAircraft::SetSpeed(const float speed) { if (IsValid(FlightComponent)) FlightComponent->SetInitialSpeed(speed); }

bool ABaseAircraft::IsLanded() { if (IsValid(FlightComponent)) return FlightComponent->IsLanded(); else return false; }

float ABaseAircraft::GetSpeed() { if (IsValid(FlightComponent)) return FlightComponent->GetCurrentSpeedKMH(); else return 0.f; }

void ABaseAircraft::SetWeapons(const TMap<FName, TSubclassOf<ABaseWeapon>> In) { if (IsValid(WeaponComponent)) WeaponComponent->SetWeapons(In); }

void ABaseAircraft::SetSpecial(const TSubclassOf<UBaseSpecial> In) { if (IsValid(SpecialComp)) SpecialComp->SetSpecial(In); }

void ABaseAircraft::SetFlightMode(const EFlightMode FlightMode)
{ 
	if (IsValid(FlightComponent)) FlightComponent->SetFlightMode(FlightMode); 
	SetLandingGearVisiblility(FlightMode != EFlightMode::Flight);
}

float ABaseAircraft::ReturnRudder() const { if (IsValid(VisualComp)) return VisualComp->GetRudder(); else return 0; }

float ABaseAircraft::ReturnSlat() const { if (IsValid(VisualComp)) return VisualComp->GetSlat(); else return 0; }

float ABaseAircraft::ReturnRFlap() const { if (IsValid(VisualComp)) return VisualComp->GetRFlap(); else return 0; }

float ABaseAircraft::ReturnLFlap() const { if (IsValid(VisualComp)) return VisualComp->GetLFlap(); else return 0; }

float ABaseAircraft::ReturnNozzle() const { if (IsValid(VisualComp)) return VisualComp->GetNozzle(); else return 0; }

float ABaseAircraft::ReturnAirbrake() const { if (IsValid(VisualComp)) return VisualComp->GetAirBrake(); else return 0; }

float ABaseAircraft::ReturnElevator() const { if (IsValid(VisualComp)) return VisualComp->GetElevator(); else return 0; }

EThrottleStage ABaseAircraft::GetThrottleStage() const { if (IsValid(FlightComponent)) return FlightComponent->ReturnThrottleStage(); else return EThrottleStage::Slow; }

void ABaseAircraft::ApplySpeed(const float Speed, const float D) { if (IsValid(FlightComponent)) FlightComponent->AddSpeed(Speed, D); }