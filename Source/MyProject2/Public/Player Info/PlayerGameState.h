// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Structs and Data/KillEvent.h"
#include "PlayerGameState.generated.h"

UCLASS()
class MYPROJECT2_API APlayerGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly)
	TArray<FKillEvent> KillLog{};

	UFUNCTION()
	void RegisterKill(AActor* Killer, AActor* Target, const TSubclassOf<AActor> WeaponUsed);
};
