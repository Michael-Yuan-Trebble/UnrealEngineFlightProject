// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/BaseWeapon.h"
#include "Structs and Data/Weapon Data/BombStats.h"
#include "BaseBomb.generated.h"

class ABaseAircraft;

UCLASS()
class MYPROJECT2_API ABaseBomb : public ABaseWeapon
{
	GENERATED_BODY()
	
public:
	ABaseBomb();

	void Drop();

	float ReturnCooldownTime() { return cooldownTime; }

	UPROPERTY(EditAnywhere)
	UBombStats* BombStats;

	FVector CurrentDirection;

	ABaseAircraft* Owner;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void CheckAndDelete(AActor* OtherActor);

	void DestroyBomb();
};
