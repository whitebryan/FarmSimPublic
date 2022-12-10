// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "Engine/Texture2D.h"
#include "InventoryItem.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct FInvTableItem : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName type;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FString> availSeasons;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class ABasePlant> plantBlueprint;
};

USTRUCT(BlueprintType, Blueprintable)
struct FInvItem
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0", ClampMax = "99", UIMin = "0", UIMax = "99"))
	int32 quantity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName type;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* icon;

	FInvItem()
	{
		name = "Empty";
		quantity = 0;
		type = "Empty";
		icon = nullptr;
	}

	bool operator==(const FInvItem& other) const {
		if (name == other.name && quantity == other.quantity) { return true; }
		else { return false; }
	}
};
