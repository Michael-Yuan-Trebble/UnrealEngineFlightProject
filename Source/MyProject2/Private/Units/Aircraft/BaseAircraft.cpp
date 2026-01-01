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
	Airframe->SetupAttachment(Collision);
	Airframe->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	LandingGear = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Landing Gear"));
	LandingGear->SetupAttachment(Airframe);
	LandingGear->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BodyCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BodyCollision"));
	BodyCollision->SetupAttachment(Collision);
	BodyCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	LeftWingCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftWingCollision"));
	LeftWingCollision->SetupAttachment(Collision);
	LeftWingCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RightWingCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightWingCollision"));
	RightWingCollision->SetupAttachment(Collision);
	RightWingCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RudderCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RudderCollision"));
	RudderCollision->SetupAttachment(Collision);
	RudderCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	LandingGearCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LandingGearCollision"));
	LandingGearCollision->SetupAttachment(Collision);
	LandingGearCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	FlightComponent = CreateDefaultSubobject<UFlightComponent>(TEXT("FlightComponent"));
	RadarComponent = CreateDefaultSubobject<URadarComponent>(TEXT("Radar"));
	WeaponComponent = CreateDefaultSubobject<UWeaponSystemComponent>(TEXT("WeaponComponent"));
	SpecialComp = CreateDefaultSubobject<USpecialSystemComponent>(TEXT("SpecialComponent"));

	PrimaryActorTick.bCanEverTick = true;
	bLocked = false;

	UnitType = ETargetType::Air;
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

	RadarComponent->Setup(this);
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
		VisualComp->SetPitch(FlightComponent->UserPitch);
		VisualComp->SetYaw(FlightComponent->UserYaw);
		VisualComp->SetRoll(FlightComponent->UserRoll);
		VisualComp->SetThrust(FlightComponent->CurrentThrust);
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

void ABaseAircraft::OnLandingGearHit(UPrimitiveComponent* HitComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hi) {
	if (bDestroyed) return;

}

void ABaseAircraft::OnBodyHit(UPrimitiveComponent* HitComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hi) {

}

void ABaseAircraft::Crash() {
	bDestroyed = true;
}

void ABaseAircraft::SetThrust(float thrust) { if (IsValid(FlightComponent)) FlightComponent->SetThrust(thrust); }

void ABaseAircraft::SetRoll(float roll) { if (IsValid(FlightComponent)) FlightComponent->SetRoll(roll); }

void ABaseAircraft::SetPitch(float pitch) { if (IsValid(FlightComponent)) FlightComponent->SetPitch(pitch); }

void ABaseAircraft::SetRudder(float rudder) { if (IsValid(FlightComponent)) FlightComponent->SetYaw(rudder); }

void ABaseAircraft::SetFlying(bool bIsFlying) { if (IsValid(FlightComponent)) FlightComponent->isFlying = bIsFlying; }

void ABaseAircraft::SetSpeed(float speed) { if (IsValid(FlightComponent)) FlightComponent->SetInitialSpeed(speed); }

void ABaseAircraft::SetWeapons(TMap<FName, TSubclassOf<ABaseWeapon>> In) { if (IsValid(WeaponComponent)) WeaponComponent->SetWeapons(In); }

void ABaseAircraft::SetSpecial(TSubclassOf<UBaseSpecial> In) { if (IsValid(SpecialComp)) SpecialComp->SetSpecial(In); }

void ABaseAircraft::SetFlightMode(EFlightMode FlightMode)
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