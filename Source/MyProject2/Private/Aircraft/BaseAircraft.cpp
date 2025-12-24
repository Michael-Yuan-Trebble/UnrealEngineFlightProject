// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Base!"));
#include "Aircraft/BaseAircraft.h"
#include "AircraftRegistry.h"
#include "Aircraft/FlightComponent.h"
#include "Aircraft/WeaponSystemComponent.h"
#include "Aircraft/AircraftVisualComponent.h"
#include "Aircraft/RadarComponent.h"
#include "Aircraft/SpecialSystemComponent.h"

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

	if (VisualCompClass) {
		VisualComp = NewObject<UAircraftVisualComponent>(this, VisualCompClass);
		if (VisualComp) {
			VisualComp->RegisterComponent();
		}
	}

	if (!Airframe || !RadarComponent || !FlightComponent) return;

	RadarComponent->Setup(this);
	FlightComponent->Setup(this, AirStats);
	WeaponComponent->Setup(this, AirStats);

	if (AfterburnerSystem) 
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
			if (tempAfterburner)
			{
				tempAfterburner->Deactivate();
				AllAfterburners.Add(tempAfterburner);
			}
		}
	}

	if (WingVortexSystem) 
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
			if (tempVortex)
			{
				tempVortex->Deactivate();
				AllVortices.Add(tempVortex);
			}
		}
	}

	FlightComponent->OnAfterburnerEngaged.AddDynamic(this, &ABaseAircraft::HandleAfterburnerFX);
	FlightComponent->OnVortexActivate.AddDynamic(this, &ABaseAircraft::HandleVortexFX);

	SetLandingGearVisiblility(bLandingGear);
}

void ABaseAircraft::PossessedBy(AController* NewController) 
{
	Super::PossessedBy(NewController);
}

void ABaseAircraft::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (VisualComp && FlightComponent) 
	{
		VisualComp->SetPitch(FlightComponent->UserPitch);
		VisualComp->SetYaw(FlightComponent->UserYaw);
		VisualComp->SetRoll(FlightComponent->UserRoll);
		VisualComp->SetThrust(FlightComponent->CurrentThrust);
	}
}

void ABaseAircraft::FireWeaponSelected() { if (WeaponComponent) WeaponComponent->FireWeaponSelected(WeaponComponent->GetWeapon()->GetClass(), Tracked, FlightComponent->GetSpeed()); }

void ABaseAircraft::HandleAfterburnerFX(bool isActive) 
{
	if (isActive) ActivateAfterburnerFX();
	else DeactivateAfterburnerFX();
}

void ABaseAircraft::ActivateAfterburnerFX() 
{
	for (UNiagaraComponent* FX : AllAfterburners) 
	{
		if (!IsValid(FX)) continue;
		FX->Activate();
	}
}

void ABaseAircraft::DeactivateAfterburnerFX() 
{
	for (UNiagaraComponent* FX : AllAfterburners)
	{
		if (!IsValid(FX)) continue;
		FX->Deactivate();
	}
}

void ABaseAircraft::HandleVortexFX(bool isActive) 
{
	if (isActive) ActivateVortexFX();
	else DeactivateVortexFX();
}

void ABaseAircraft::ActivateVortexFX() 
{
	for (UNiagaraComponent* FX : AllVortices) 
	{
		if (!IsValid(FX)) continue;
		FX->Activate();
	}
}

void ABaseAircraft::DeactivateVortexFX() 
{
	for (UNiagaraComponent* FX : AllVortices) 
	{
		if (!IsValid(FX)) continue;
		FX->Deactivate();
	}
}

void ABaseAircraft::DisableAllMainWingVapors() {
	for (UStaticMeshComponent* Mesh : AllMainWingVapors) {
		if (!IsValid(Mesh)) continue;
		Mesh->SetVisibility(false);
	}
}

void ABaseAircraft::EnableAllMainWingVapors() {

}

void ABaseAircraft::HandleLOD(FVector CameraLoc) 
{
	// Setting distance to KM
	if (!Airframe) return;
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

void ABaseAircraft::SetThrust(float thrust) { if (FlightComponent) FlightComponent->SetThrust(thrust); }

void ABaseAircraft::SetRoll(float roll) { if (FlightComponent) FlightComponent->SetRoll(roll); }

void ABaseAircraft::SetPitch(float pitch) { if (FlightComponent) FlightComponent->SetPitch(pitch); }

void ABaseAircraft::SetRudder(float rudder) { if (FlightComponent) FlightComponent->SetYaw(rudder); }

void ABaseAircraft::SetFlying(bool bIsFlying) { if (FlightComponent) FlightComponent->isFlying = bIsFlying; }

void ABaseAircraft::SetSpeed(float speed) { 
	if (FlightComponent) 
	{
		FlightComponent->currentSpeed = speed;
		FlightComponent->Velocity = this->GetActorForwardVector() * speed;
	}
}

void ABaseAircraft::SetWeapons(TMap<FName, TSubclassOf<ABaseWeapon>> In) { if (WeaponComponent) WeaponComponent->SetWeapons(In); }

void ABaseAircraft::SetSpecial(UBaseSpecial* In) { if (SpecialComp) SpecialComp->SetSpecial(In); }

float ABaseAircraft::ReturnRudder() const { if (VisualComp) return VisualComp->GetRudder(); else return 0; }

float ABaseAircraft::ReturnSlat() const { if (VisualComp) return VisualComp->GetSlat(); else return 0; }

float ABaseAircraft::ReturnRFlap() const { if (VisualComp) return VisualComp->GetRFlap(); else return 0; }

float ABaseAircraft::ReturnLFlap() const { if (VisualComp) return VisualComp->GetLFlap(); else return 0; }

float ABaseAircraft::ReturnNozzle() const { if (VisualComp) return VisualComp->GetNozzle(); else return 0; }

float ABaseAircraft::ReturnAirbrake() const { if (VisualComp) return VisualComp->GetAirBrake(); else return 0; }

float ABaseAircraft::ReturnElevator() const { if (VisualComp) return VisualComp->GetElevator(); else return 0; }