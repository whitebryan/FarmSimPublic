// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "InputCoreTypes.h"
#include "PlayerMappableInputConfig.h"
#include "InventoryItem.h"
#include "BasePlant.h"
#include "DLSSLibrary.h"
#include "PlayerSaveGame.generated.h"

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

/**
 * 
 */
UCLASS()
class FARMSIM_API UPlayerSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TMap<FName, FKey> inputMap;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	float playerSensitivity = 1.25;



	UPROPERTY()
	TArray<FInvItem> playerInventory;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Inventory")
	int numRows = 1;

	UPROPERTY()
	TArray<FChestSaveStruct> placedChests;

	UPROPERTY()
	TArray<FActorSaveStruct> plantedCrops;
	UPROPERTY()
	TArray<FActorSaveStruct> growthPlots;
	UPROPERTY()
	TArray<FActorSaveStruct> harvestables;

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
	TArray<URecipeAsset*> knownRecipes;

	UPROPERTY(BlueprintReadOnly)
	TMap<FName, UToolItemAsset*> currentTools;

	UPROPERTY(BlueprintReadOnly)
	TArray<FName> constructedBuildings;

	UPROPERTY(BlueprintReadOnly)
	TArray<FVector> brokenPieces;

	UPROPERTY(BlueprintReadOnly)
	TMap<FName, FFishStatSaveStruct> fishStats;

	UPROPERTY(BlueprintReadOnly)
	TArray<UItemAsset*> discoveredItems;
};
