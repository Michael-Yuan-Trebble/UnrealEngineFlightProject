// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("AI CONTROL!"));
#include "Units/Aircraft/AI/EnemyAircraftAI.h"
#include "Engine/World.h"
#include "Units/Aircraft/AI/EnemyAircraft.h"
#include "GameFramework/PlayerController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Units/Aircraft/BaseAircraft.h"
#include "Units/Aircraft/WeaponSystemComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapons/BaseWeapon.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

AEnemyAircraftAI::AEnemyAircraftAI() 
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemyAircraftAI::BeginPlay() 
{
	Super::BeginPlay();
}

void AEnemyAircraftAI::OnPossess(APawn* PawnPossess) 
{
	Super::OnPossess(PawnPossess);
	if (BehaviorTreeAsset) 
	{
		if (UseBlackboard(BehaviorTreeAsset->BlackboardAsset, BlackboardComp)) 
		{
			RunBehaviorTree(BehaviorTreeAsset);
			BlackboardComp->SetValueAsBool(TEXT("bIsAttacking"), false);
			BlackboardComp->SetValueAsFloat(TEXT("PitchAmount"), 0.f);
			BlackboardComp->SetValueAsFloat(TEXT("RollAmount"), 0.f);
			BlackboardComp->SetValueAsFloat(TEXT("YawAmount"), 0.f);
			BlackboardComp->SetValueAsFloat(TEXT("ThrottleAmount"), 0.f);
		}
	}

	Controlled = Cast<AEnemyAircraft>(GetPawn());
	if (!Controlled) return;

	WeaponComp = Controlled->WeaponComponent;
}

void AEnemyAircraftAI::Tick(float DeltaTime) 
{
	Super::Tick(DeltaTime);
}

void AEnemyAircraftAI::Weapons(TSubclassOf<ABaseWeapon> WeaponClass, AActor* Selected, float Speed)
{
	if (!IsValid(Controlled)) return;
	Controlled->FireWeaponSelected();
}

void AEnemyAircraftAI::SwitchWeapon(TSubclassOf<ABaseWeapon> InWeapon) 
{
	if (IsValid(Controlled)) Controlled->SwitchWeapon(InWeapon);
}

void AEnemyAircraftAI::ShootGun(bool bShoot) 
{
	if (!IsValid(Controlled) || bShoot == bIsFiring) return;
	if (bShoot) Controlled->StartBullets();
	else Controlled->EndBullets();
	bIsFiring = bShoot;
}

void AEnemyAircraftAI::SetThrust(const float Thrust) { if (Controlled) Controlled->SetThrust(Thrust); }

void AEnemyAircraftAI::SetPitch(const float Pitch) { if (Controlled) Controlled->SetPitch(Pitch); }

void AEnemyAircraftAI::SetRoll(const float Roll) { if (Controlled) Controlled->SetRoll(Roll); }

void AEnemyAircraftAI::SetYaw(const float Yaw) { if (Controlled) Controlled->SetRudder(Yaw); }

void AEnemyAircraftAI::SetFlying(bool bIsFlying) { if (Controlled) Controlled->SetFlying(bIsFlying); }

void AEnemyAircraftAI::Special() {

}

void AEnemyAircraftAI::SpecialCooldown() {

}
