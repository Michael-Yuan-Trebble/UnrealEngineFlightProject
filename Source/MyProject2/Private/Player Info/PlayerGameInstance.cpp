// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Instance!"));
#include "Player Info/PlayerGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Gamemodes/MainMenuManager.h"
#include "UI/Transition/FadeWidget.h"
#include "Subsystem/LevelTransitionSubsystem.h"
#include "UI/Transition/TransitionWidget.h"
#include "Subsystem/MissionManagerSubsystem.h"
#include "Player Info/SaveGameManager.h"

void UPlayerGameInstance::Init() 
{
	Super::Init();
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(
		this,
		&UPlayerGameInstance::HandlePostLoad
	);
	SaveManager = NewObject<USaveGameManager>(this, USaveGameManager::StaticClass());
	SaveManager->LoadGame();
	UMissionManagerSubsystem* Sub = GetSubsystem<UMissionManagerSubsystem>();
	if (IsValid(Sub)) 
	{
		Sub->SetNavalCarrierMap(NavalCarrierMap);
		Sub->SetDefaultMap(DefaultMap);
		Sub->SetAircraftSelectMap(AircraftSelectMap);
		Sub->SetGroundTakeoffMap(GroundTakeoffMap);
	}
}

void UPlayerGameInstance::FadeIn() 
{
	if (!DoesFadeExist()) CreateFade();
	if (!FadeWidget) return;

	FadeWidget->OnFadeFinished.AddUniqueDynamic(this, &UPlayerGameInstance::HandleFadeFinished);
	FadeWidget->PlayFadeIn();
}

void UPlayerGameInstance::HandleFadeFinished() {
	if (UMissionManagerSubsystem* MissionSub = GetSubsystem<UMissionManagerSubsystem>()) {
		if (ULevelTransitionSubsystem* LevelSub = GetSubsystem<ULevelTransitionSubsystem>()) {
			LevelSub->LoadMission(MissionSub->GetCurrentMission().Level);
		}
	}
}

void UPlayerGameInstance::HandlePostLoad(UWorld* LoadedWorld) {
	if (FadeWidget) FadeWidget->AddToViewport(9999);
	else CreateFade();
	FadeOut();
}

void UPlayerGameInstance::FadeOut()
{
	if (!DoesFadeExist()) CreateFade();
	if (!FadeWidget) return;

	FadeWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
	FadeWidget->PlayFadeOut();
}

void UPlayerGameInstance::CreateFade() {
	if (!FadeWidgetClass) return;
	FadeWidget = CreateWidget<UFadeWidget>(this, FadeWidgetClass);
	if (FadeWidget) 
	{
		FadeWidget->AddToViewport(1000);
		FadeWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void UPlayerGameInstance::ShowTransition() {
	if (!DoesTransitionExist()) CreateTransition();
	if (!TransitionWidget) return;

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;
	
	TransitionWidget->SetVisibility(ESlateVisibility::Visible);
	PC->SetInputMode(FInputModeUIOnly());
	TransitionWidget->SetKeyboardFocus();
	TransitionWidget->StartScreen();
}

void UPlayerGameInstance::HideTransition() {
	if (!DoesTransitionExist()) CreateTransition();
	if (!TransitionWidget) return;

	TransitionWidget->HideScreen();
}

void UPlayerGameInstance::CreateTransition() {
	if (!IsValid(TransitionClass)) return;
	TransitionWidget = CreateWidget<UTransitionWidget>(this, TransitionClass);
	if (IsValid(TransitionWidget)) {
		TransitionWidget->AddToViewport(100);
		TransitionWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void UPlayerGameInstance::GoToLevel() {
	ULevelTransitionSubsystem* LevelSubsystem = GetSubsystem<ULevelTransitionSubsystem>();
	UMissionManagerSubsystem* MissionSubsystem = GetSubsystem<UMissionManagerSubsystem>();

	if (!LevelSubsystem || !MissionSubsystem) return;
	LevelSubsystem->LoadIntermission(MissionSubsystem->GetCurrentMission().TakeoffType);
}

void UPlayerGameInstance::SetLevel(const FMissionData& InLevel) {
	UMissionManagerSubsystem* MissionSubsystem = GetSubsystem<UMissionManagerSubsystem>();

	if (!IsValid(MissionSubsystem)) return;
	MissionSubsystem->SetCurrentMission(Level);
}

bool UPlayerGameInstance::DoesTransitionExist() const {
	return IsValid(TransitionWidget);
}

bool UPlayerGameInstance::DoesFadeExist() const {
	return IsValid(FadeWidget);
}

void UPlayerGameInstance::SetAircraftSelectMap() {
	if (UMissionManagerSubsystem* MissionSubsystem = GetSubsystem<UMissionManagerSubsystem>()) {
		MissionSubsystem->SetAircraftSelectMap(AircraftSelectMap);
	}
}

void UPlayerGameInstance::SetNavalCarrierMap() {
	if (UMissionManagerSubsystem* MissionSubsystem = GetSubsystem<UMissionManagerSubsystem>()) {
		MissionSubsystem->SetNavalCarrierMap(NavalCarrierMap);
	}
}

void UPlayerGameInstance::SetGroundTakeoffMap() {
	if (UMissionManagerSubsystem* MissionSubsystem = GetSubsystem<UMissionManagerSubsystem>()) {
		MissionSubsystem->SetGroundTakeoffMap(GroundTakeoffMap);
	}
}

void UPlayerGameInstance::SetDefaultMap() {
	if (UMissionManagerSubsystem* MissionSubsystem = GetSubsystem<UMissionManagerSubsystem>()) {
		MissionSubsystem->SetDefaultMap(DefaultMap);
	}
}