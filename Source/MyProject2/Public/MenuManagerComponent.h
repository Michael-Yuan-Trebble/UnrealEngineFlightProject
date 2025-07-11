// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structs and Data/MenuState.h"
#include "UI/AircraftSelectionWidget.h"
#include "UI/WeaponSelectionWidget.h"
#include "UI/BuyPopupWidget.h"
#include "Structs and Data/AircraftDatabase.h"
#include "MenuManagerComponent.generated.h"

class AAircraftPlayerController;
class ACurrentPlayerState;
class AAircraftSelectionGamemode;
class UPlayerGameInstance;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT2_API UMenuManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMenuManagerComponent();

	void InitializePC(AAircraftPlayerController* InPC, ACurrentPlayerState* InPS);

	void GoBack();

	void GetWidgetClassForState(EMenuState State);

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAircraftSelectionWidget* AircraftSelectUI;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> WeaponSelectClass;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UWeaponSelectionWidget* WeaponSelectUI;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> BuyPopupClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBuyPopupWidget* BuyWidget;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> GreyOutClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UUserWidget* GreyOut;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBuyPopupWidget* BuyPopupUI;

	UPROPERTY()
	UAircraftDatabase* AircraftDatabase;

	AAircraftPlayerController* PC;

	ACurrentPlayerState* PS;

	AAircraftSelectionGamemode* GM;

	UPlayerGameInstance* GameInstance;

	TMap<FName, TSubclassOf<ABaseWeapon>> WeaponSelection;

	UAircraftData* SelectedAircraft;

	int32 CurrentPylonIndex;

	void ChooseAircraftUI();

	void ChooseWeaponUI();

	void ChooseSpecialUI();

	UFUNCTION()
	void SetAircraft(UAircraftData* AircraftData);

	UFUNCTION()
	void AddWeapon(TSubclassOf<ABaseWeapon> Weapon);

	UFUNCTION()
	void SpawnBuy();

	UFUNCTION()
	void BuyAircraft(FName Name, int Cost);

	UFUNCTION()
	void CancelBuy();

	void AddDatabase();


protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleAircraftPicked(TSubclassOf<APawn> Aircraft);

	UFUNCTION()
	void HandleWeaponPicked(TSubclassOf<ABaseWeapon> Weapon);

private:	

	void AircraftSelectionMenu();

	void WeaponSelectionMenu();

	void SpecialSelectionMenu();

	void CheckWeaponLoop();

	UUserWidget* CurrentWidget;

	TArray<EMenuState> MenuHistory;

	EMenuState CurrentState;
};
