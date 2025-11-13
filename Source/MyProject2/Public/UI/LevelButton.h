// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "LevelButton.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelPicked, FName, SelectedLevel);

UCLASS()
class MYPROJECT2_API ULevelButton : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnLevelPicked OnLevelPicked;

	UPROPERTY(EditAnywhere)
	FName LevelName;

	void SetupLevel(FName InName);

protected:
	virtual void NativeConstruct() override;

private:

	UFUNCTION()
	void HandleButtonClick();

	UPROPERTY(meta = (BindWidget))
	UButton* LevelSelectbtn;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LevelText;
};
