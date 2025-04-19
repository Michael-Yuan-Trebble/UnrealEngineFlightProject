// Fill out your copyright notice in the Description page of Project Settings.


#include "FDetectedAircraftInfo.h"

FDetectedAircraftInfo::FDetectedAircraftInfo()
	: Location(FVector::ZeroVector)
	, Rotation(FRotator::ZeroRotator)
	, CurrentPawn(nullptr)
	, threatLevel(0.f)
{



}