// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "LocationStatus.generated.h"

UENUM(Blueprintable, BlueprintType)
enum LocationStatus
{
	Valley UMETA(DisplayName = "Valley"),
	Ocean UMETA(DisplayName = "Ocean"),
	Beach UMETA(DisplayName = "Beach"),
	Pier UMETA(DisplayName = "Pier"),
	Harbor UMETA(DisplayName = "Harbor"),
	BrokenBridge UMETA(DisplauName = "Collapsed Bridge"),
	Park UMETA(DisplayName = "Park"),
	City UMETA(DisplayName = "City"),
	Foundry UMETA(DisplayName = "Foundry"),
	Radiation UMETA(DisplayName = "Irradiated Zone"),
	Interior UMETA(DisplayName = "Interior"),
	Home UMETA(DisplayName = "Home"),
	HomeYard UMETA(DisplayName = "Garden"),
	Anywhere UMETA(DisplayName = "Anywhere"),
};
