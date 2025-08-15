// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Database!"));
#include "Structs and Data/AircraftDatabase.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/AssetManager.h"

void UAircraftDatabase::LoadAllAircraftFromFolder(FString Path) 
{
    AllAircraft.Empty();

    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    FARFilter Filter;
    Filter.PackagePaths.Add(FName(*Path));
    Filter.bRecursivePaths = true;

    TArray<FAssetData> Assets;
    AssetRegistryModule.Get().GetAssets(Filter, Assets);
    for (const FAssetData& Data : Assets) 
    {
        UBlueprint* BlueprintAsset = Cast<UBlueprint>(Data.GetAsset());
        if (!BlueprintAsset)
        {
            UObject* LoadedObject = StaticLoadObject(UBlueprint::StaticClass(), nullptr, *Data.ObjectPath.ToString());
            BlueprintAsset = Cast<UBlueprint>(LoadedObject);
        }

        if (BlueprintAsset && BlueprintAsset->GeneratedClass)
        {
            if (BlueprintAsset->GeneratedClass->IsChildOf(UAircraftData::StaticClass()))
            {
                
                UObject* CDO = BlueprintAsset->GeneratedClass->GetDefaultObject();
                UAircraftData* AircraftData = Cast<UAircraftData>(CDO);
                if (!AircraftData) continue;
                AllAircraft.Add(AircraftData);
            }
        }
    }
}