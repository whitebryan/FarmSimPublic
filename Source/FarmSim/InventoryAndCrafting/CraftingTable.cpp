// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftingTable.h"
#include "../Player/FarmSimCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ACraftingTable::ACraftingTable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ACraftingTable::BeginPlay()
{
	Super::BeginPlay();

	AFarmSimCharacter* player = Cast<AFarmSimCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	playerSaveManager = Cast<UPlayerSaveManagerComponent>(player->GetComponentByClass(UPlayerSaveManagerComponent::StaticClass()));
	playerInv = Cast<UInventoryComponent>(player->GetComponentByClass(UInventoryComponent::StaticClass()));
}

// Called every frame
void ACraftingTable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACraftingTable::craftItem(URecipeAsset* recipeToCraft, int quantity)
{
	//go through inventories to remove ingredients, prioritizing chests
	TMap<UItemAsset*, int> neededIngredients;
	neededIngredients = recipeToCraft->ingredientsNeeded;

	TArray<UItemAsset*> keys;

	for (UInventoryComponent* curInv : inventories)
	{
		//Regrab keys every time incase something is no longer needed
		neededIngredients.GetKeys(keys);


		for (UItemAsset* key : keys)
		{
			int amtNeeded = neededIngredients[key] * quantity;

			int amtFound = curInv->getItemQuantity(key->uniqueID);
			if (amtFound > 0)
			{
				if (amtFound >= amtNeeded)
				{
					curInv->changeQuantity(key->uniqueID, -amtNeeded);
					neededIngredients.Remove(key);
				}
				else if (amtFound > 0)
				{
					curInv->changeQuantity(key->uniqueID, -amtFound);
					neededIngredients.Add(key, amtNeeded - amtFound);
				}
			}
		}
	}

	//If items are still needed now remove them from player inv
	if (neededIngredients.Num() > 0)
	{
		neededIngredients.GetKeys(keys);
		for (UItemAsset* key : keys)
		{
			int amtNeeded = neededIngredients[key] * quantity;

			int amtFound = playerInv->getItemQuantity(key->uniqueID);
			if (amtFound > 0)
			{
				if (amtFound >= amtNeeded)
				{
					playerInv->changeQuantity(key->uniqueID, -amtNeeded);
					neededIngredients.Remove(key);
				}
			}
		}
	}

	if (recipeToCraft->recipeType == "Tool")
	{
		UToolItemAsset* newTool = Cast<UToolItemAsset>(recipeToCraft->itemToMake);

		Cast<AFarmSimCharacter>(playerInv->GetOwner())->changeTool(recipeToCraft->itemToMake->type, newTool);
	}
	else if(recipeToCraft->recipeType != "Building")
	{
		//make FInvItem to add to inv
		FInvItem itemToAdd;
		itemToAdd.item = recipeToCraft->itemToMake;
		itemToAdd.quantity = quantity * recipeToCraft->amountMade;

		//Add to player Inv or drop if full
		playerInv->addNewItem(itemToAdd, true, true);
	}
}

//Calculate how many of each item is craftable
TMap<URecipeAsset*, int> ACraftingTable::generateCratableAmounts(const FName type)
{
	TMap<URecipeAsset*, int> results;
	TArray<URecipeAsset*> knowRecipes = playerSaveManager->learnedRecipes;

	//Go through all known recipes and calculate how man the player can make based off ingredients in inv and all chests
	for (URecipeAsset* id : knowRecipes)
	{
		if (id->recipeType != type)
		{
			continue;
		}
		
		//Make sure the players tool is the worse than this tool recipe
		if (id->recipeType == "tool")
		{
			AFarmSimCharacter* player = Cast<AFarmSimCharacter>(playerInv->GetOwner());
			UToolItemAsset* newTool = Cast<UToolItemAsset>(id->itemToMake);
			UToolItemAsset* playerTool = player->grabTool(newTool->type);

			if (playerTool->toolTier >= newTool->toolTier)
			{
				continue;
			}
		}

		FRecipeTableItem* curRecipe = recipeTable->FindRow<FRecipeTableItem>(id->uniqueID, FString(""));
		TMap<UItemAsset*, int> neededIngredients = curRecipe->recipe->ingredientsNeeded;
		TMap<UItemAsset*, int> ingredientsFound;

		TArray<UItemAsset*> keys;
		neededIngredients.GetKeys(keys);

		for (UInventoryComponent* curInv : inventories)
		{
			for (UItemAsset* key : keys)
			{
				int amtFound = curInv->getItemQuantity(key->uniqueID);

				if (ingredientsFound.Contains(key))
				{
					ingredientsFound.Add(key, ingredientsFound[key] + amtFound);
				}
				else
				{
					ingredientsFound.Add(key, amtFound);
				}
			}
		}

		for (UItemAsset* key : keys)
		{
			int amtFound = playerInv->getItemQuantity(key->uniqueID);

			if (ingredientsFound.Contains(key))
			{
				ingredientsFound.Add(key, ingredientsFound[key] + amtFound);
			}
			else
			{
				ingredientsFound.Add(key, amtFound);
			}
		}

		int maxCrafts = -1;
		for (UItemAsset* key : keys)
		{
			if (!ingredientsFound.Contains(key))
			{
				maxCrafts = -1;
				break;
			}

			int cur = ingredientsFound[key] / neededIngredients[key];

			if (ingredientsFound[key] < neededIngredients[key])
			{
				maxCrafts = -1;
				break;
			}
			else if (maxCrafts == -1 || cur < maxCrafts)
			{
				maxCrafts = cur;
			}
		}

		if (maxCrafts == -1)
		{
			results.Add(id, 0);
		}
		else
		{
			results.Add(id, maxCrafts);
		}
	}
	return results;
}


void ACraftingTable::Interact(bool status)
{
	if (status)//If opening get all crafting table UI gather all of the inventory components
	{
		toggleUI(true);
		grabChests();
	}
	else//Else clear them to save memory
	{
		toggleUI(false);
		inventories.Empty();
	}
}


void ACraftingTable::grabChests()
{
	TArray<AActor*> chests;

	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "Storage", chests);

	for (int i = 0; i < chests.Num(); ++i)
	{
		UInventoryComponent* curInv = Cast<UInventoryComponent>(chests[i]->GetComponentByClass(UInventoryComponent::StaticClass()));

		if (IsValid(curInv))
		{
			inventories.Add(curInv);
		}
	}
}
