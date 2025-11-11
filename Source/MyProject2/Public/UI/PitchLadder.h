// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "PitchLadder.generated.h"

UCLASS()
class MYPROJECT2_API UPitchLadder : public UUserWidget
{
	GENERATED_BODY()

public:
	void Update(float InPitch, FVector NoseDir, FVector CameraLoc, bool Project);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* LadderCanvas;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pitch Ladder")
	UMaterialInterface* LineTexture;

private:
	UPROPERTY()
	TArray<UImage*> LadderLines;

	UPROPERTY()
	TArray<UTextBlock*> LadderLabels;

	float CurrentPitch = 0.f;

	const int32 PitchStep = 10;
	const int32 MinPitch = -90;
	const int32 MaxPitch = 90;
};
