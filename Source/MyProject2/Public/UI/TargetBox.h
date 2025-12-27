// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TargetBox.generated.h"

UCLASS()
class MYPROJECT2_API ATargetBox : public AActor
{
	GENERATED_BODY()
	
public:	
	ATargetBox();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
