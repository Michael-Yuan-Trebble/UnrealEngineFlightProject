// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelListData.generated.h"

USTRUCT(BlueprintType)
struct FLevelInfo {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName LevelName;

	UPROPERTY(EditAnywhere)
	FText DisplayName;

	UPROPERTY(EditAnywhere)
	UTexture2D* PreviewImage;
};

UCLASS()
class MYPROJECT2_API ULevelListData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	TArray<FLevelInfo> Levels;
};
