// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpecialSystemComponent.generated.h"

class UBaseSpecial;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT2_API USpecialSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USpecialSystemComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetSpecial(UBaseSpecial* In) { Special = In; };

	void Activate();

	void Deactivate();

private:
	UBaseSpecial* Special;
};
