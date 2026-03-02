// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Structs and Data/MissionData.h"
#include "LevelListData.generated.h"

USTRUCT(BlueprintType)
struct FLevelInfo {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FMissionData LevelData{};

	UPROPERTY(EditAnywhere)
	FText DisplayName = FText::GetEmpty();

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> PreviewImage = nullptr;
};

UCLASS()
class MYPROJECT2_API ULevelListData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	TArray<FLevelInfo> Levels{};
};
