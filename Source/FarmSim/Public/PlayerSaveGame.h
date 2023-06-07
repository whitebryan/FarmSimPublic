// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "InputCoreTypes.h"
#include "PlayerMappableInputConfig.h"
#include "InventoryAndCrafting/InventoryItem.h"
#include "Farming/BasePlant.h"
#include "DLSSLibrary.h"
#include "Quests/QuestComponent.h"
#include "PlayerSaveGame.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FClothesSaveStruct
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UModularClothingAsset* clothesAsset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor pieceColor;
};

USTRUCT(Blueprintable, BlueprintType)
struct FChestSaveStruct
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FInvItem> chestInventory;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int numRows = 1;
	UPROPERTY()
	FTransform chestTransform;

	FORCEINLINE bool operator==(FChestSaveStruct& Other) 
	{
		if(chestTransform.GetLocation() == Other.chestTransform.GetLocation())
			return true;
		else
			return false;
	}
};

USTRUCT(Blueprintable)
struct FActorSaveStruct
{
	GENERATED_BODY()

	UPROPERTY()
	TSubclassOf<AActor> actorType;
	UPROPERTY()
	float actorTimer;
	UPROPERTY()
	FTransform actorTransform;
};

USTRUCT(BlueprintType, Blueprintable)
struct FFishStatSaveStruct
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	float curMin = 999;
	UPROPERTY(BlueprintReadOnly)
	float curMax = -1;
};

USTRUCT(BlueprintType, Blueprintable)
struct FHarvestableSaveStruct : public FActorSaveStruct
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	TArray<FTransform> childTransforms;

	UPROPERTY(BlueprintReadOnly)
	TMap<FVector, float> childTimers;
};


/**
 * 
 */
UCLASS()
class FARMSIM_API UPlayerSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	TArray<FInvItem> playerInventory;
	UPROPERTY(BlueprintReadOnly, Category = "Player Inventory")
	int numRows = 1;

	UPROPERTY()
	TArray<FChestSaveStruct> placedChests;

	UPROPERTY()
	TArray<FActorSaveStruct> plantedCrops;
	UPROPERTY()
	TArray<FActorSaveStruct> growthPlots;
	UPROPERTY()
	TMap<FVector, FHarvestableSaveStruct> harvestables;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UDLSSMode playerDLSSMode;

	UPROPERTY()
	int autoSaveTime = 5;

	UPROPERTY(BlueprintReadOnly)
	FName lastWeather = "clear";
	UPROPERTY(BlueprintReadOnly)
	float timeOfDay = 900;
	UPROPERTY(BlueprintReadOnly)
	float season = 3.5;
	UPROPERTY(BlueprintReadOnly)
	int daysIntoSeason = 0;
	UPROPERTY(BlueprintReadOnly)
	int dayOfTheWeek = 0;

	UPROPERTY(BlueprintReadOnly)
	TArray<URecipeAsset*> knownRecipes;

	UPROPERTY(BlueprintReadOnly)
	TMap<FGameplayTag, UToolItemAsset*> currentTools;

	UPROPERTY(BlueprintReadOnly)
	TArray<FName> constructedBuildings;

	UPROPERTY(BlueprintReadOnly)
	TArray<FVector> brokenPieces;

	UPROPERTY(BlueprintReadOnly)
	TMap<FName, FFishStatSaveStruct> fishStats;

	UPROPERTY(BlueprintReadOnly)
	TArray<UItemAsset*> discoveredItems;

	UPROPERTY(BlueprintReadOnly)
	TMap<TEnumAsByte<ClothesCategory>, FClothesSaveStruct> playerClothes;

	UPROPERTY(BlueprintReadOnly)
	FLinearColor skinColor;

	UPROPERTY(BlueprintReadOnly)
	TMap<FString, int> npcConversations;
	UPROPERTY(BlueprintReadOnly)
	TMap<FString, FQuest> playerQuests;

	UPROPERTY(BlueprintReadOnly)
	FGameplayTagContainer playerTags;
};
