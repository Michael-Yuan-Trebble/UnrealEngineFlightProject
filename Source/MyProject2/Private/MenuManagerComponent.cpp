// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("MenuManager!"));
#include "MenuManagerComponent.h"
#include "GameFramework/SpectatorPawn.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGameManager.h"
#include "Gamemodes/PlayerGameInstance.h"
#include "Gamemodes/AircraftSelectionGamemode.h"
#include "AircraftPlayerController.h"

UMenuManagerComponent::UMenuManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetBPClass(TEXT("/Game/UI/BPAircraftSelectionWidget"));
	if (WidgetBPClass.Succeeded())
	{
		MainMenuWidgetClass = WidgetBPClass.Class;
	}
	static ConstructorHelpers::FClassFinder<UUserWidget> WeaponBPClass(TEXT("/Game/UI/BPWeaponSelectWidget"));
	if (WeaponBPClass.Succeeded()) 
	{
		WeaponSelectClass = WeaponBPClass.Class;
	}
	static ConstructorHelpers::FClassFinder<UUserWidget> BuyBPClass(TEXT("/Game/UI/BPBuyPopupWidget"));
	if (BuyBPClass.Succeeded()) {
		BuyPopupClass = BuyBPClass.Class;
	}
	static ConstructorHelpers::FClassFinder<UUserWidget> GreyOutBPClass(TEXT("/Game/UI/UIDim"));
	if (GreyOutBPClass.Succeeded()) {
		GreyOutBPClass.Class;
	}
}


void UMenuManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentPylonIndex = 0;
	GameInstance = Cast<UPlayerGameInstance>(GetWorld()->GetGameInstance());
}

void UMenuManagerComponent::InitializePC(AAircraftPlayerController* InPC, ACurrentPlayerState* InPS)
{
	PC = InPC;
	PS = InPS;
	GM = Cast<AAircraftSelectionGamemode>(UGameplayStatics::GetGameMode(this));
}

void UMenuManagerComponent::AddDatabase() 
{
	AircraftDatabase = NewObject<UAircraftDatabase>(PC->GetGameInstance());
	if (!AircraftDatabase) return;

	FString Path = "/Game/Aircraft/AircraftData";
	AircraftDatabase->LoadAllAircraftFromFolder(Path);
}

void UMenuManagerComponent::GoBack()
{

}

void UMenuManagerComponent::GetWidgetClassForState(EMenuState State)
{
	switch (State) 
	{
	case EMenuState::AircraftSelect:
		if (!GM) return;
		AircraftSelectionMenu();
		PC->SetWidget(AircraftSelectUI);
	case EMenuState::WeaponSelect:
		if (!SelectedAircraft) return;
		WeaponSelectionMenu();
		PC->SetWidget(WeaponSelectUI);
	case EMenuState::SpecialSelect:
		return;
	default:
		return;
	}
}

void UMenuManagerComponent::AircraftSelectionMenu() 
{
	if (PC == nullptr) return;

	AircraftSelectUI = CreateWidget<UAircraftSelectionWidget>(GetWorld(), MainMenuWidgetClass);

	if (AircraftSelectUI && AircraftDatabase)
	{
		AircraftSelectUI->AircraftDatabase = AircraftDatabase;
		AircraftSelectUI->Owned = GameInstance->SaveManager->GetAircraftOwned();
		AircraftSelectUI->CurrentMoney = GameInstance->SaveManager->ReturnMoney();
		AircraftSelectUI->GetAllAircraft();
		AircraftSelectUI->AddToViewport();
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(AircraftSelectUI->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(true);
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
		CurrentWidget = AircraftSelectUI;
		AircraftSelectUI->OnAircraftSelected.AddDynamic(this, &UMenuManagerComponent::HandleAircraftPicked);
	}
}

void UMenuManagerComponent::HandleAircraftPicked(TSubclassOf<APawn> Aircraft) 
{
	if (!Aircraft || !PC) return;
	GM->SpawnInAircraft(Aircraft);
}

void UMenuManagerComponent::SetAircraft(UAircraftData* Aircraft)
{
	if (!Aircraft) return;

	SelectedAircraft = Aircraft;
	PS->SetCurrentAircraft(Aircraft);
	ChooseWeaponUI();
}

void UMenuManagerComponent::WeaponSelectionMenu() 
{
	if (PC == nullptr) return;

	WeaponSelectUI = CreateWidget<UWeaponSelectionWidget>(GetWorld(), WeaponSelectClass);
	if (!WeaponSelectUI) return;

	WeaponSelectUI->CurrentLoadout = &(SelectedAircraft->PylonLoadouts[CurrentPylonIndex]);
	WeaponSelectUI->GetAllAircraft();
	WeaponSelectUI->AddToViewport();
	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(WeaponSelectUI->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(true);
	PC->SetInputMode(InputMode);
	PC->bShowMouseCursor = true;
	CurrentWidget = WeaponSelectUI;
	WeaponSelectUI->OnWeaponSelected.AddDynamic(this, &UMenuManagerComponent::HandleWeaponPicked);
}

void UMenuManagerComponent::HandleWeaponPicked(TSubclassOf<ABaseWeapon> Weapon) 
{
	if (!Weapon || !PC) return;
	FString PylonString = FString::Printf(TEXT("Pylon%d"), CurrentPylonIndex);
	FName PylonName = FName(*PylonString);
	GM->SpawnInWeapon(Weapon, PylonName);
}

void UMenuManagerComponent::AddWeapon(TSubclassOf<ABaseWeapon> Weapon) 
{
	FString PylonString = FString::Printf(TEXT("Pylon%d"), CurrentPylonIndex);
	FName PylonName = FName(*PylonString);
	WeaponSelection.Add(PylonName, Weapon);
	CheckWeaponLoop();
}

void UMenuManagerComponent::CheckWeaponLoop()
{
	if (CurrentPylonIndex + 1 >= SelectedAircraft->NumOfPylons) ChooseSpecialUI(); else
	{
		CurrentPylonIndex++;
		PC->ManageMenuSetting(EMenuState::WeaponSelect);
	}
}

void UMenuManagerComponent::SpecialSelectionMenu() 
{

}

void UMenuManagerComponent::SpawnBuy() 
{
	GreyOut = CreateWidget<UUserWidget>(PC, GreyOutClass);
	if (!GreyOut) return;
	GreyOut->AddToViewport(0);

	BuyWidget = CreateWidget<UBuyPopupWidget>(PC, BuyPopupClass);
	if (!BuyWidget) return;
	BuyWidget->Setup(SelectedAircraft);
	BuyWidget->AddToViewport(1);

	//Put a new menu state for buy widget so that controls work
}

void UMenuManagerComponent::BuyAircraft(FName Name, int Cost) 
{

}

void UMenuManagerComponent::CancelBuy() 
{

}

void UMenuManagerComponent::ChooseAircraftUI() 
{
	PC->ManageMenuSetting(EMenuState::AircraftSelect);
}

void UMenuManagerComponent::ChooseWeaponUI() 
{
	PC->ManageMenuSetting(EMenuState::WeaponSelect);
}

void UMenuManagerComponent::ChooseSpecialUI() 
{
	PC->ManageMenuSetting(EMenuState::SpecialSelect);
}