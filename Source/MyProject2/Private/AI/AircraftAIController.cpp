// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AircraftAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Units/Aircraft/BaseAircraft.h"


AAircraftAIController::AAircraftAIController() {
	PrimaryActorTick.bCanEverTick = true;
}

void AAircraftAIController::BeginPlay() {
	Super::BeginPlay();
}

void AAircraftAIController::OnPossess(APawn* PawnPossess) {
	Super::OnPossess(PawnPossess);
	if (BehaviorTreeAsset) {
		if (UseBlackboard(BehaviorTreeAsset->BlackboardAsset, BlackboardComp)) {
			RunBehaviorTree(BehaviorTreeAsset);
			BlackboardComp->SetValueAsBool(TEXT("bIsAttacking"), false);
			BlackboardComp->SetValueAsFloat(TEXT("PitchAmount"), 0.f);
			BlackboardComp->SetValueAsFloat(TEXT("RollAmount"), 0.f);
			BlackboardComp->SetValueAsFloat(TEXT("YawAmount"), 0.f);
			BlackboardComp->SetValueAsFloat(TEXT("ThrottleAmount"), 0.f);
		}
	}

	Controlled = Cast<ABaseAircraft>(GetPawn());
	if (!Controlled) return;
}

void AAircraftAIController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AAircraftAIController::Weapons() {
	if (IsValid(Controlled)) Controlled->FireWeaponSelected();
}

void AAircraftAIController::SwitchWeapon(TSubclassOf<ABaseWeapon> InWeapon) {
	if (IsValid(Controlled)) Controlled->SwitchWeapon(InWeapon);
}

void AAircraftAIController::ShootGun(const bool bShoot) {
	if (!IsValid(Controlled) || bShoot == bIsFiring) return;
	if (bShoot) Controlled->StartBullets();
	else Controlled->EndBullets();
	bIsFiring = bShoot;
}

void AAircraftAIController::SetThrust(const float Thrust) { if (IsValid(Controlled)) Controlled->SetThrust(Thrust); }

void AAircraftAIController::SetPitch(const float Pitch) { if (IsValid(Controlled)) Controlled->SetPitch(Pitch); }

void AAircraftAIController::SetRoll(const float Roll) { if (IsValid(Controlled)) Controlled->SetRoll(Roll); }

void AAircraftAIController::SetYaw(const float Yaw) { if (IsValid(Controlled)) Controlled->SetRudder(Yaw); }

void AAircraftAIController::SetFlying(const bool bIsFlying) { if (IsValid(Controlled)) Controlled->SetFlying(bIsFlying); }

void AAircraftAIController::Special() {

}