// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CountermeasureActor.generated.h"

UCLASS()
class MYPROJECT2_API ACountermeasureActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ACountermeasureActor();

	void Activate();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<class UNiagaraComponent> FlareComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Events")
	TObjectPtr<class UNiagaraSystem> FlareEffect = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> RootMesh = nullptr;

	FTimerHandle LifetimeTimer;

	float Lifetime = 5;

	UPROPERTY(EditAnywhere)
	float Repitions = 0.f;

	UPROPERTY(EditAnywhere)
	float AmountPerRepition = 0.f;
};
