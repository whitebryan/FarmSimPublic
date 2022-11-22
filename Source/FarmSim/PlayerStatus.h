// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerStatus.generated.h"

UENUM(Blueprintable, BlueprintType)
enum PlayerStatus
{
	NormalState UMETA(DisplayName = "NormalState"),
	Planting UMETA(DisplayName = "Planting"),
	InStorage UMETA(DisplayName = "InStorage"),
	Fishing UMETA(DisplayName = "Fishing"),
};

UENUM(Blueprintable, BlueprintType)
enum PlayerToolStatus
{
	NoToolOut = 0 UMETA(DisplayName = "NoToolOut"),
	PickaxeOut = 1 UMETA(DisplayName = "PickaxeOut"),
	AxeOut = 2 UMETA(DisplayName = "AxeOut"),
	ShovelOut = 3 UMETA(DisplayName = "ShovelOut"),
	WateringCanOut = 4 UMETA(DisplayName = "WateringCanOut"),
	FishingRodOut = 5 UMETA(DisplayName = "FishingRodOut"),
};
