// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Base!"));
#include "Aircraft/BaseAircraft.h"
#include "AircraftRegistry.h"
#include "Aircraft/FlightComponent.h"
#include "Aircraft/WeaponSystemComponent.h"
#include "Aircraft/RadarComponent.h"

ABaseAircraft::ABaseAircraft()
{
	Airframe = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Airframe"));
	Airframe->SetupAttachment(Collision);
	Airframe->SetCollisionEnabled(ECollisionEnabled::NoCollision);

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

	FlightComponent = CreateDefaultSubobject<UFlightComponent>(TEXT("FlightComponent"));
	RadarComponent = CreateDefaultSubobject<URadarComponent>(TEXT("Radar"));

	PrimaryActorTick.bCanEverTick = true;
	bLocked = false;

	UnitType = ETargetType::Air;
}

void ABaseAircraft::BeginPlay()
{
	Super::BeginPlay();

	RadarComponent->Setup(this);
	FlightComponent->Setup(this, AirStats);

	if (!AfterburnerSystem) return;
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

	FlightComponent->OnAfterburnerEngaged.AddDynamic(this, &ABaseAircraft::HandleAfterburnerFX);
}

void ABaseAircraft::PossessedBy(AController* NewController) 
{
	Super::PossessedBy(NewController);
}

void ABaseAircraft::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (FlightComponent->ReturnThrottleStage() == EThrottleStage::Afterburner) 
	{
		ActivateAfterburnerFX();
	} 
	else if (FlightComponent->ReturnThrottleStage() != EThrottleStage::Afterburner) 
	{
		DeactivateAfterburnerFX();
	}
}

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