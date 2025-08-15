// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PlayerGameInstance.generated.h"

UCLASS()
class MYPROJECT2_API UPlayerGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UPROPERTY()
	class USaveGameManager* SaveManager;

protected:
	void Init() override;
};
