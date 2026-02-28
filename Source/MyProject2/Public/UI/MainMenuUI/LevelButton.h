// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Structs and Data/MissionData.h"
#include "LevelButton.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelPicked, FMissionData, SelectedLevel);

UCLASS()
class MYPROJECT2_API ULevelButton : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnLevelPicked OnLevelPicked;

	void SetupLevel(const FMissionData& InData);

protected:
	virtual void NativeConstruct() override;

private:

	UPROPERTY(EditAnywhere)
	FMissionData MissionData{};

	UFUNCTION()
	void HandleButtonClick();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> LevelSelectbtn = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LevelText = nullptr;
};
