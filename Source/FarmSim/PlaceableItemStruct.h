// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "Engine/Texture2D.h"
#include "LocationStatus.h"
#include "InventoryItem.h"
#include "PlaceableItemStruct.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct FPlaceableItem : public FInvItem
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TEnumAsByte<LocationStatus>> placeableBiome;

	FPlaceableItem()
	{
	}
};
