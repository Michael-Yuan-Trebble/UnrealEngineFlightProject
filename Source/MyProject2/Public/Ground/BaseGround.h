// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseUnit.h"
#include "BaseGround.generated.h"

UCLASS()
class MYPROJECT2_API ABaseGround : public ABaseUnit
{
	GENERATED_BODY()
	
public:	
	ABaseGround();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Mesh;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

};
