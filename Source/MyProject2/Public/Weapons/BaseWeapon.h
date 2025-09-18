// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "BaseWeapon.generated.h"

UCLASS()
class MYPROJECT2_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	
	ABaseWeapon();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UBoxComponent* Collision;

	FName WeaponName;

	float cooldownTime;

	float timeTilDelt;

	float timeDet;

	bool canLock;

	float range;

	virtual void FireStatic(float speed);

	virtual void FireTracking(float speed, AActor* Target);

protected:
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

};
