// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#if !UE_BUILD_SHIPPING

#define AIR_DEBUG_KEY(Key, Color, Format, ...)\
do { \
	if (GEngine) \
	{ \
	GEngine->AddOnScreenDebugMessage( \
		Key, \
		0.1f, \
		Color, \
		FString::Printf(TEXT(Format), ##__VA_ARGS__) \
	); \
	} \
} while(0)

#define DEBUG_TIME(Time, Format,...)\
do { \
	if(GEngine) \
	{\
	GEngine->AddOnScreenDebugMessage( \
		-1, \
		Time, \
		FColor::Yellow, \
		FString::Printf(TEXT(Format), ##__VA_ARGS__) \
	); \
	} \
} while(0)

#else

// Shipping Build

#define AIR_DEBUG_KEY(...)
#define DEBUG_LOCATION(...)
#endif