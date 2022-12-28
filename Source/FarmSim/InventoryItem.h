// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "Engine/Texture2D.h"
#include "LocationStatus.h"
#include "InventoryItem.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct FAddItemStatus
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	bool addStatus = false;
	UPROPERTY(BlueprintReadOnly)
	int leftOvers = 0;
};

USTRUCT(BlueprintType, Blueprintable)
struct FInvTableItem : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName type;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TEnumAsByte<LocationStatus>> placeableBiome;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FString> availSeasons;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class AActor> itemBlueprint;
};

USTRUCT(BlueprintType, Blueprintable)
struct FInvItem
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName uniqueID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName type;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0", ClampMax = "99", UIMin = "0", UIMax = "99"))
	int32 quantity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* icon;

	FInvItem()
	{
		uniqueID = "Empty";
		name = "Empty";
		quantity = 0;
		type = "Empty";
		icon = nullptr;
	}

	bool operator==(const FInvItem& other) const {
		if (uniqueID == other.uniqueID) { return true; }
		else { return false; }
	}
};
