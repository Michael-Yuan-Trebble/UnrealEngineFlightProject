// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EnhancedInputComponent.h"
#include "MenuInputConfig.generated.h"

UCLASS()
class MYPROJECT2_API UMenuInputConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> IA_Up = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> IA_Down = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> IA_Back = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> IA_Menu = nullptr;

};
