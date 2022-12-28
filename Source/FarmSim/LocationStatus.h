// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "LocationStatus.generated.h"

UENUM(Blueprintable, BlueprintType)
enum LocationStatus
{
	Valley UMETA(DisplayName = "Valley"),
	Ocean UMETA(DisplayName = "Ocean"),
	Home UMETA(DisplayName = "Home"),
};
