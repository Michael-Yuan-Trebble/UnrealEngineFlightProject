// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Units/Aircraft/Player/PlayerAircraft.h"
#include "Su25Pawn.generated.h"

UCLASS()
class MYPROJECT2_API ASu25Pawn : public APlayerAircraft
{
	GENERATED_BODY()
	
	public:

		ASu25Pawn();

		//Clamp to 35
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float RudderTopYaw;

		//Clamp to 30
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float RudderBottomYaw;

		//Clamp to -80
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float AirbrakeTop;

		//Clamp to 80
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float AirbrakeBottom;

		//Clamp to -1.2, 15, 0.5
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRotator OuterFlap;

		//Clamp to 0.1, 20, 0
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRotator MiddleFlap;

		//Clamp to 20
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float InnerFlap;

		//Clamp to 0, 20, -2
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRotator ElevatorMain;

		//Clamp to Translation(5,0,-12) and 20
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector ElevatorSub;

		//Clamp to 90
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float FrontWheelCoverTop;

		//Clamp to 90
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float FrontWheelCoverBottom;

		//Clamp to 90
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float FrontWheel;
		
		//Clamp to -90
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float BackWheelCoverSide;

		//Clamp to 90
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float BackWheelCoverBody;

		//Clamp to -90
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float BackWheel;

		virtual void PossessedBy(AController* NewController);

	protected:

		virtual void Tick(float DeltaTime) override;
		virtual void BeginPlay() override;

	private:
		
		void PitchElevatorCalculation(float DeltaSeconds);
		void RollElevatorCalculation(float DeltaSeconds);
		void SlatsElevationCalculation(float DeltaSeconds);
		void RudderYawCalculation(float DeltaSeconds);
		void WheelCalculation(float DeltaSeconds);
		void AirbrakeCalculation(float DeltaSeconds);

};
