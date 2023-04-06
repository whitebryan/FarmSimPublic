// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "Engine/Texture2D.h"
#include "../ToolItem.h"
#include "GameplayTagContainer.h"
#include "InventoryItem.generated.h"


//General Data table stuff
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
	UItemAsset* item;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer placeableBiome;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class AActor> itemBlueprint;
};
//

UENUM(Blueprintable, BlueprintType)
enum ClothesCategory
{
	Hair UMETA(DisplayName = "Hair"),
	Shirt UMETA(DisplayName = "Shirt"),
	Pants UMETA(DisplayName = "Pants"),
	Shoes UMETA(DisplayName = "Shoes"),
	Gloves UMETA(DisplayName = "Gloves"),
	Accessory UMETA(DisplayName = "Accessory"),
};

UCLASS(BlueprintType, Blueprintable)
class FARMSIM_API UModularClothingAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USkeletalMesh* clothingMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<ClothesCategory> category;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName name;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsColorable = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* icon = nullptr;
};

USTRUCT(BlueprintType, Blueprintable)
struct FClothingTableItem : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UModularClothingAsset* clothingAsset;
};

//Base item asset used for most things
UCLASS(BlueprintType)
class FARMSIM_API UItemAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName uniqueID = "-1";

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName name = "Empty";

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString description = "Empty";

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName type = "None";

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* icon = nullptr;
};
//

//Seed data asset
UCLASS(BlueprintType)
class FARMSIM_API USeedItemAsset : public UItemAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FString> availSeasons;
};

//Plant data asset
UCLASS(BlueprintType)
class FARMSIM_API UPlantItemAsset : public UItemAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "1", ClampMax = "999", UIMin = "1", UIMax = "999", ToolTip = "Real time seconds to full growth"))
	int secondsToGrow = 99999;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "1", UIMin = "1", ToolTip = "Minimun amount harvested per harvest"))
	int harvestMin = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ToolTip = "Maximum amount to harvest per harvest"))
	int harvestMax = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "1", ClampMax = "5", UIMin = "1", UIMax = "5", ToolTip = "How many times you will need to water the plant after the inital watering broken into even chunks"))
	int timesToWater = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ToolTip = "The unwatered version of the plant"))
	UStaticMesh* unwateredModel;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ToolTip = "The watered but non grown version of the plant"))
	UStaticMesh* wateredGrowingModel;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ToolTip = "The grown version of the plant"))
	UStaticMesh* grownModel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FString> availSeasons;
};
//

//Harvestable data asset
UCLASS(BlueprintType)
class FARMSIM_API UHarvestableItemAsset : public UItemAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int harvestAmount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool requiresTool = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag requiredToolStatus = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<ToolTier> toolLevel = ToolTier::Bronze;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMesh* harvestedModel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* harvestableModel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool respawnAble = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int respawnTime = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip = "The locations this can be harvested."))
	FGameplayTagContainer harvestableAreas;

};
//

//Fish data asset
UCLASS(BlueprintType)
class FARMSIM_API UFishItemAsset : public UItemAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip = "The model of the fish"))
	UStaticMesh* fishModel;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip = "The location the fish is catchable."))
	FGameplayTag catchableArea;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip = "The seasons the fish is catchable."))
	FName seasons = "Summer&Winter";
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip = "The weather the fish appears in."));
	FName weather = "Any";
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip = "Minimum length the fish can drop at in cm"))
	int minLenght = 10;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip = "Maximum length the fish can drop at in cm"))
	int maxLength = 25;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip = "Time the fish starts appearing"))
	int startTime = 99;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip = "Time the fish stops appearing"))
	int doneTime = 99;

};

//Tool data asset
UCLASS(BlueprintType)
class FARMSIM_API UToolItemAsset : public UItemAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag toolStatus = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMesh* model;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<ToolTier> toolTier;

};


//
USTRUCT(BlueprintType, Blueprintable)
struct FInvItem
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UItemAsset* item;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0", ClampMax = "99", UIMin = "0", UIMax = "99"))
	int32 quantity;

	FInvItem()
	{
		item = nullptr;
		quantity = 0;
	}

	bool operator==(const FInvItem& other) const 
	{

		if (item->uniqueID == other.item->uniqueID) { return true; }
		else { return false; }
	}
};
//





//Crafting Data Assets
UCLASS(BlueprintType)
class FARMSIM_API URecipeAsset : public UItemAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName recipeType = "General";
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<UItemAsset* , int> ingredientsNeeded;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int amountMade = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UItemAsset* itemToMake;
};

USTRUCT(BlueprintType, Blueprintable)
struct FRecipeTableItem : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	URecipeAsset* recipe;
};