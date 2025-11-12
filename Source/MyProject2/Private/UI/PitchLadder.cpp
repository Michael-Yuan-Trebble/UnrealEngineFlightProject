// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PitchLadder.h"
#include "Components/CanvasPanelSlot.h"

void UPitchLadder::NativeConstruct() 
{
	Super::NativeConstruct();

	if (!LadderCanvas || !LineTexture) return;

	for (int32 Angle = MinPitch; Angle <= MaxPitch; Angle += PitchStep) 
    {
		UImage* Line = NewObject<UImage>(this, UImage::StaticClass());
        //Line->SetBrushFromTexture(LineTexture);
        Line->SetBrushFromMaterial(LineTexture);
        Line->SetDesiredSizeOverride(FVector2D(3.f,130.f));
        Line->SetVisibility(ESlateVisibility::Visible);

        UCanvasPanelSlot* CanvasSlot = LadderCanvas->AddChildToCanvas(Line);
        CanvasSlot->SetZOrder(1);
        CanvasSlot->SetAutoSize(false);
        CanvasSlot->SetAnchors(FAnchors(0, 0));
        CanvasSlot->SetPosition(FVector2D(100.f, 0.f));

        LadderLines.Add(Line);

        UTextBlock* Label = NewObject<UTextBlock>(this, UTextBlock::StaticClass());
        Label->SetText(FText::AsNumber(Angle));
        Label->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
        CanvasSlot = LadderCanvas->AddChildToCanvas(Label);
        CanvasSlot->SetAutoSize(true);
        CanvasSlot->SetAnchors(FAnchors(0, 0));
        CanvasSlot->SetPosition(FVector2D(50.f, 0.f));
        LadderLabels.Add(Label);
	}
}

void UPitchLadder::Update(float InPitch) 
{
    if (!LadderCanvas || !IsVisible()) return;

    LadderCanvas->SetRenderTransformAngle(0.f);

    CurrentPitch = InPitch;

    FVector2D CanvasSize = LadderCanvas->GetCachedGeometry().GetLocalSize();
    float LadderHeight = CanvasSize.Y;
    float CenterY = LadderHeight / 2.f;

    float VisibleRange = 30.f;
    float PixelsPerDegree = LadderHeight / VisibleRange;
    bool bFlip = FMath::Abs(CurrentPitch) > 90.f;

    for (int32 i = 0; i < LadderLines.Num(); ++i)
    {
        float LineAngle = MinPitch + i * PitchStep;
        float PitchDiff = LineAngle - CurrentPitch;

        bool bVisible = FMath::Abs(PitchDiff) <= (30.f / 2.f);
        LadderLines[i]->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
        LadderLabels[i]->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

        if (!bVisible) continue;

        float DisplayAngle = bFlip ? -LineAngle : LineAngle;
        LadderLabels[i]->SetText(FText::AsNumber(DisplayAngle));
        float Y = CenterY - (PitchDiff * PixelsPerDegree);

        if (UCanvasPanelSlot* LineSlot = Cast<UCanvasPanelSlot>(LadderLines[i]->Slot))
        {
           float X = LineSlot->GetPosition().X;
           LineSlot->SetPosition(bFlip ? FVector2D(-X, Y) : FVector2D(X,Y));
        }

        if (UCanvasPanelSlot* LabelSlot = Cast<UCanvasPanelSlot>(LadderLabels[i]->Slot))
        {
           float X = LabelSlot->GetPosition().X;
           LabelSlot->SetPosition(bFlip ? FVector2D(-X, Y) : FVector2D(X, Y));
        }
    }
}