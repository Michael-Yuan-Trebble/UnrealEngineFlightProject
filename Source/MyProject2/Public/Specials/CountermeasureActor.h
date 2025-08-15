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

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UNiagaraComponent* FlareComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Events")
	class UNiagaraSystem* FlareEffect;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* RootMesh;

	FTimerHandle LifetimeTimer;

	float Lifetime = 5;

	UFUNCTION()
	void OnLifetimeExpired();
};
