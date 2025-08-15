// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

UCLASS()
class MYPROJECT2_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	
	ABaseWeapon();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* WeaponMesh;

	FName WeaponName;

	float cooldownTime;

	float timeTilDelt;

	float timeDet;

protected:
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

};
