// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "InputCoreTypes.h"
#include "PlayerMappableInputConfig.h"
#include "InventoryItem.h"
#include "PlayerSaveGame.generated.h"

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Inventory")
	TArray<FInvItem> inventoryArray;
};
