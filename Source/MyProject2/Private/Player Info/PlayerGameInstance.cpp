// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Instance!"));
#include "Player Info/PlayerGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Gamemodes/MainMenuManager.h"
#include "FadeWidget.h"
#include "Player Info/SaveGameManager.h"

void UPlayerGameInstance::Init() 
{
	Super::Init();
	SaveManager = NewObject<USaveGameManager>(this, USaveGameManager::StaticClass());
	SaveManager->LoadGame();
}

void UPlayerGameInstance::FadeIn() 
{
	if (!DoesFadeExist()) CreateFade();
	if (!FadeWidget) return;

	FadeWidget->PlayFadeIn();
}

void UPlayerGameInstance::FadeOut()
{
	if (!DoesFadeExist()) CreateFade();
	if (!FadeWidget) return;

	FadeWidget->SetVisibility(ESlateVisibility::Visible);
	FadeWidget->PlayFadeOut();
}

void UPlayerGameInstance::CreateFade() {
	if (!FadeWidget && FadeWidget) 
	{
		FadeWidget = CreateWidget<UFadeWidget>(this, FadeWidgetClass);
		if (FadeWidget) 
		{
			FadeWidget->AddToViewport(1000);
			FadeWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

bool UPlayerGameInstance::DoesFadeExist() const {
	return IsValid(FadeWidget);
}