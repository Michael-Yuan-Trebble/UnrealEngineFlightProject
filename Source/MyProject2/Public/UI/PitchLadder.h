// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PitchLadder.generated.h"

UCLASS()
class MYPROJECT2_API UPitchLadder : public UUserWidget
{
	GENERATED_BODY()

public:
	void Update(const float InPitch);

private:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> LadderCanvas = nullptr;

	UPROPERTY(EditAnywhere, Category="Pitch Ladder")
	TObjectPtr<UMaterialInterface> LineTexture = nullptr;

	UPROPERTY()
	TArray<class UImage*> LadderLines{};

	UPROPERTY()
	TArray<class UTextBlock*> LadderLabels{};

	float CurrentPitch = 0.f;

	const int32 PitchStep = 10;
	const int32 MinPitch = -90;
	const int32 MaxPitch = 90;
};
