// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "LocationSeasonalItemMap.generated.h"

//Simple struct made to be able to map Location -> Season -> Difficulty Table
USTRUCT(Blueprintable, BlueprintType)
struct FLocationSeasonalItemMapStruct 
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<FString, UDataTable*> seasonalMap;

	FLocationSeasonalItemMapStruct()
	{
	}
};