#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationEditorCommon.h"
#include "InventoryComponent.h"
#include "EngineUtils.h" 
#include "Engine/World.h"
#include "CraftingTable.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(AddItemEmptyTest, "Inventory&Crafting.Add Empty", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(AddItemFullTest, "Inventory&Crafting.Add Full Drop", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(AddItemPartialTest, "Inventory&Crafting.Add Drop Partially", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(RemoveItemEmptyTest, "Inventory&Crafting.Remove Item Empty", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(RemoveItemFullStackTest, "Inventory&Crafting.Remove Full Stack", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(RemoveItemPartialStackTest, "Inventory&Crafting.Remove Partial Stack", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(AddRowsTest, "Inventory&Crafting.Add New Rows", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)


//Test adding to an empty inventory
bool AddItemEmptyTest::RunTest(const FString& Parameters)
{
	FAutomationEditorCommonUtils::CreateNewMap();
	UWorld* World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();
	//FAutomationEditorCommonUtils::CreateNewMap();
	//FAutomationEditorCommonUtils::LoadMap
	//FAutomationEditorCommonUtils::RunPIE

	FInvItem newInvItem;
	newInvItem.quantity = 99;

	FStringAssetReference itemAssetPath("/Game/Blueprints/Plants/DataAssets/Seeds/WheatSeed.WheatSeed");
	UObject* itemObject = itemAssetPath.TryLoad();
	UItemAsset* itemAsset = Cast<UItemAsset>(itemObject);
	if (!IsValid(itemAsset))
	{
		AddError(TEXT("Could not load inv item"));
		return false;
	}
	newInvItem.item = itemAsset;
	
	FStringAssetReference chestAssetPath("/Game/Blueprints/Interactables/Placeables/Chest.Chest");
	UObject* chestObject = chestAssetPath.TryLoad();
	if (!IsValid(chestObject))
	{
		AddError(TEXT("Could not load chest asset"));
		return false;
	}

	{
		AActor* newActor = World->SpawnActor<AActor>(Cast<UBlueprint>(chestObject)->GeneratedClass, FVector(0, 0, 0), UKismetMathLibrary::MakeRotator(0, 0, 0));
		if (!IsValid(newActor))
		{
			AddError(TEXT("Actor didn't spawn"));
			return false;
		}
		else
		{
			UInventoryComponent* newInv = Cast<UInventoryComponent>(newActor->GetComponentByClass(UInventoryComponent::StaticClass()));
			newInv->addNewItem(newInvItem);

			if (newInv->getItemQuantity(newInvItem.item->uniqueID) != 99)
			{
				AddError(TEXT("Item did not add"));
				return false;
			}

			return true;
		}
	}
}

//Test adding to a full inventory with no space at all
bool AddItemFullTest::RunTest(const FString& Parameters)
{
	FAutomationEditorCommonUtils::CreateNewMap();
	FAutomationEditorCommonUtils::LoadMap("AutomatedTestsMap");
	UWorld* World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();//FAutomationEditorCommonUtils::CreateNewMap();

	FInvItem newInvItem;
	newInvItem.quantity = 99;

	FStringAssetReference itemAssetPath("/Game/Blueprints/Plants/DataAssets/Seeds/WheatSeed.WheatSeed");
	UObject* itemObject = itemAssetPath.TryLoad();
	UItemAsset* itemAsset = Cast<UItemAsset>(itemObject);
	if (!IsValid(itemAsset))
	{
		AddError(TEXT("Could not load inv item"));
		return false;
	}
	newInvItem.item = itemAsset;

	FStringAssetReference chestAssetPath("/Game/Blueprints/Interactables/Placeables/Chest.Chest");
	UObject* chestObject = chestAssetPath.TryLoad();
	if (!IsValid(chestObject))
	{
		AddError(TEXT("Could not load chest asset"));
		return false;
	}
	
	FStringAssetReference lootBagAssetPath("/Game/Blueprints/Interactables/LootBag.LootBag");
	UObject* lootBagObject = lootBagAssetPath.TryLoad();
	if (!IsValid(lootBagObject))
	{
		AddError(TEXT("Could not load loot bag asset"));
		return false;
	}

	{
		AActor* newActor = World->SpawnActor<AActor>(Cast<UBlueprint>(chestObject)->GeneratedClass, FVector(0, 0, 0), UKismetMathLibrary::MakeRotator(0, 0, 0));
		if (!IsValid(newActor))
		{
			AddError(TEXT("Actor didn't spawn"));
			return false;
		}
		else
		{
			UInventoryComponent* newInv = Cast<UInventoryComponent>(newActor->GetComponentByClass(UInventoryComponent::StaticClass()));
			newInv->addNewItem(newInvItem);
			newInv->addNewItem(newInvItem);
			newInv->addNewItem(newInvItem);
			newInv->addNewItem(newInvItem);
			newInv->addNewItem(newInvItem);

			if (newInv->getItemQuantity(newInvItem.item->uniqueID) != 99 * 5)
			{
				AddError(TEXT("Items did not fully add"));
				return false;
			}

			newInv->addNewItem(newInvItem, true);

			TSubclassOf<AActor> lootBagClass = Cast<UBlueprint>(lootBagObject)->GeneratedClass;
			TArray<AActor*> lootBags;
			UGameplayStatics::GetAllActorsOfClass(World, lootBagClass, lootBags);

			if (lootBags.Num() == 0)
			{
				AddError(TEXT("No loot bags found"));
				return false;
			}

			UInventoryComponent* lootBagInv = Cast<UInventoryComponent>(lootBags[0]->GetComponentByClass(UInventoryComponent::StaticClass()));
			if (lootBagInv->getItemQuantity(newInvItem.item->uniqueID) != 99)
			{
				AddError(TEXT("Loot bag does not contain the full stack"));
				return false;
			}

			return true;
		}
	}
}


//Test adding to a mostly full inventory only being able to add part of the stack
bool AddItemPartialTest::RunTest(const FString& Parameters)
{
	FAutomationEditorCommonUtils::CreateNewMap();
	FAutomationEditorCommonUtils::LoadMap("AutomatedTestsMap");
	UWorld* World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();//FAutomationEditorCommonUtils::CreateNewMap();

	FInvItem newInvItem;
	newInvItem.quantity = 99;

	FStringAssetReference itemAssetPath("/Game/Blueprints/Plants/DataAssets/Seeds/WheatSeed.WheatSeed");
	UObject* itemObject = itemAssetPath.TryLoad();
	UItemAsset* itemAsset = Cast<UItemAsset>(itemObject);
	if (!IsValid(itemAsset))
	{
		AddError(TEXT("Could not load inv item"));
		return false;
	}
	newInvItem.item = itemAsset;

	FStringAssetReference chestAssetPath("/Game/Blueprints/Interactables/Placeables/Chest.Chest");
	UObject* chestObject = chestAssetPath.TryLoad();
	if (!IsValid(chestObject))
	{
		AddError(TEXT("Could not load chest asset"));
		return false;
	}

	FStringAssetReference lootBagAssetPath("/Game/Blueprints/Interactables/LootBag.LootBag");
	UObject* lootBagObject = lootBagAssetPath.TryLoad();
	if (!IsValid(lootBagObject))
	{
		AddError(TEXT("Could not load loot bag asset"));
		return false;
	}

	{
		AActor* newActor = World->SpawnActor<AActor>(Cast<UBlueprint>(chestObject)->GeneratedClass, FVector(0, 0, 0), UKismetMathLibrary::MakeRotator(0, 0, 0));
		if (!IsValid(newActor))
		{
			AddError(TEXT("Actor didn't spawn"));
			return false;
		}
		else
		{
			UInventoryComponent* newInv = Cast<UInventoryComponent>(newActor->GetComponentByClass(UInventoryComponent::StaticClass()));
			newInv->addNewItem(newInvItem);
			newInv->addNewItem(newInvItem);
			newInv->addNewItem(newInvItem);
			newInv->addNewItem(newInvItem);

			newInvItem.quantity = 50;
			newInv->addNewItem(newInvItem);

			if (newInv->getItemQuantity(newInvItem.item->uniqueID) != 99 * 4 + 50)
			{
				AddError(TEXT("Items did not fully add"));
				return false;
			}

			newInvItem.quantity = 99;
			FAddItemStatus addStatus = newInv->addNewItem(newInvItem, true, true);

			TSubclassOf<AActor> lootBagClass = Cast<UBlueprint>(lootBagObject)->GeneratedClass;
			TArray<AActor*> lootBags;
			UGameplayStatics::GetAllActorsOfClass(World, lootBagClass, lootBags);

			if (lootBags.Num() == 0)
			{
				AddError(TEXT("No loot bags found"));
				return false;
			}

			UInventoryComponent* lootBagInv = Cast<UInventoryComponent>(lootBags[0]->GetComponentByClass(UInventoryComponent::StaticClass()));
			if (lootBagInv->getItemQuantity(newInvItem.item->uniqueID) != 50)
			{
				AddError(TEXT("Loot bag does not contain the full stack"));
				return false;
			}

			if (addStatus.leftOvers != 50)
			{
				AddError(TEXT("Add status returned wrong remainder"));
				return false;
			}

			return true;
		}
	}
}

//Trying to remove an item that isnt in the inventory
bool RemoveItemEmptyTest::RunTest(const FString& Parameters)
{
	FAutomationEditorCommonUtils::CreateNewMap();
	UWorld* World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();

	FInvItem newInvItem;
	newInvItem.quantity = -99;

	FStringAssetReference itemAssetPath("/Game/Blueprints/Plants/DataAssets/Seeds/WheatSeed.WheatSeed");
	UObject* itemObject = itemAssetPath.TryLoad();
	UItemAsset* itemAsset = Cast<UItemAsset>(itemObject);
	if (!IsValid(itemAsset))
	{
		AddError(TEXT("Could not load inv item"));
		return false;
	}
	newInvItem.item = itemAsset;

	FStringAssetReference chestAssetPath("/Game/Blueprints/Interactables/Placeables/Chest.Chest");
	UObject* chestObject = chestAssetPath.TryLoad();
	if (!IsValid(chestObject))
	{
		AddError(TEXT("Could not load chest asset"));
		return false;
	}

	FStringAssetReference lootBagAssetPath("/Game/Blueprints/Interactables/LootBag.LootBag");
	UObject* lootBagObject = lootBagAssetPath.TryLoad();
	if (!IsValid(lootBagObject))
	{
		AddError(TEXT("Could not load loot bag asset"));
		return false;
	}

	{
		AActor* newActor = World->SpawnActor<AActor>(Cast<UBlueprint>(chestObject)->GeneratedClass, FVector(0, 0, 0), UKismetMathLibrary::MakeRotator(0, 0, 0));
		if (!IsValid(newActor))
		{
			AddError(TEXT("Actor didn't spawn"));
			return false;
		}
		else
		{
			UInventoryComponent* newInv = Cast<UInventoryComponent>(newActor->GetComponentByClass(UInventoryComponent::StaticClass()));
			newInv->addNewItem(newInvItem, true, true);

			if (newInv->getItemQuantity(newInvItem.item->uniqueID) != 0)
			{
				AddError(TEXT("Item was incorrectly added"));
				return false;
			}

			TSubclassOf<AActor> lootBagClass = Cast<UBlueprint>(lootBagObject)->GeneratedClass;
			TArray<AActor*> lootBags;
			UGameplayStatics::GetAllActorsOfClass(World, lootBagClass, lootBags);

			if (lootBags.Num() != 0)
			{
				AddError(TEXT("Item was incorrectly dropped"));
				return false;
			}

			return true;
		}
	}
}

//Removing a full stack
bool RemoveItemFullStackTest::RunTest(const FString& Parameters)
{
	FAutomationEditorCommonUtils::CreateNewMap();
	UWorld* World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();

	FInvItem newInvItem;
	newInvItem.quantity = 99;

	FStringAssetReference itemAssetPath("/Game/Blueprints/Plants/DataAssets/Seeds/WheatSeed.WheatSeed");
	UObject* itemObject = itemAssetPath.TryLoad();
	UItemAsset* itemAsset = Cast<UItemAsset>(itemObject);
	if (!IsValid(itemAsset))
	{
		AddError(TEXT("Could not load inv item"));
		return false;
	}
	newInvItem.item = itemAsset;

	FStringAssetReference chestAssetPath("/Game/Blueprints/Interactables/Placeables/Chest.Chest");
	UObject* chestObject = chestAssetPath.TryLoad();
	if (!IsValid(chestObject))
	{
		AddError(TEXT("Could not load chest asset"));
		return false;
	}

	FStringAssetReference lootBagAssetPath("/Game/Blueprints/Interactables/LootBag.LootBag");
	UObject* lootBagObject = lootBagAssetPath.TryLoad();
	if (!IsValid(lootBagObject))
	{
		AddError(TEXT("Could not load loot bag asset"));
		return false;
	}

	{
		AActor* newActor = World->SpawnActor<AActor>(Cast<UBlueprint>(chestObject)->GeneratedClass, FVector(0, 0, 0), UKismetMathLibrary::MakeRotator(0, 0, 0));
		if (!IsValid(newActor))
		{
			AddError(TEXT("Actor didn't spawn"));
			return false;
		}
		else
		{
			UInventoryComponent* newInv = Cast<UInventoryComponent>(newActor->GetComponentByClass(UInventoryComponent::StaticClass()));
			newInv->addNewItem(newInvItem, true, true);
			newInv->addNewItem(newInvItem, true, true);
			newInvItem.quantity = -99;
			newInv->addNewItem(newInvItem, true, true);

			if (newInv->getItemQuantity(newInvItem.item->uniqueID) != 99)
			{
				AddError(TEXT("Too much was removed"));
				return false;
			}

			TSubclassOf<AActor> lootBagClass = Cast<UBlueprint>(lootBagObject)->GeneratedClass;
			TArray<AActor*> lootBags;
			UGameplayStatics::GetAllActorsOfClass(World, lootBagClass, lootBags);

			if (lootBags.Num() != 0)
			{
				AddError(TEXT("Item was incorrectly dropped"));
				return false;
			}

			return true;
		}
	}
}

//Removing part of a stack
bool RemoveItemPartialStackTest::RunTest(const FString& Parameters)
{
	FAutomationEditorCommonUtils::CreateNewMap();
	UWorld* World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();

	FInvItem newInvItem;
	newInvItem.quantity = 99;

	FStringAssetReference itemAssetPath("/Game/Blueprints/Plants/DataAssets/Seeds/WheatSeed.WheatSeed");
	UObject* itemObject = itemAssetPath.TryLoad();
	UItemAsset* itemAsset = Cast<UItemAsset>(itemObject);
	if (!IsValid(itemAsset))
	{
		AddError(TEXT("Could not load inv item"));
		return false;
	}
	newInvItem.item = itemAsset;

	FStringAssetReference chestAssetPath("/Game/Blueprints/Interactables/Placeables/Chest.Chest");
	UObject* chestObject = chestAssetPath.TryLoad();
	if (!IsValid(chestObject))
	{
		AddError(TEXT("Could not load chest asset"));
		return false;
	}

	FStringAssetReference lootBagAssetPath("/Game/Blueprints/Interactables/LootBag.LootBag");
	UObject* lootBagObject = lootBagAssetPath.TryLoad();
	if (!IsValid(lootBagObject))
	{
		AddError(TEXT("Could not load loot bag asset"));
		return false;
	}

	{
		AActor* newActor = World->SpawnActor<AActor>(Cast<UBlueprint>(chestObject)->GeneratedClass, FVector(0, 0, 0), UKismetMathLibrary::MakeRotator(0, 0, 0));
		if (!IsValid(newActor))
		{
			AddError(TEXT("Actor didn't spawn"));
			return false;
		}
		else
		{
			UInventoryComponent* newInv = Cast<UInventoryComponent>(newActor->GetComponentByClass(UInventoryComponent::StaticClass()));
			newInv->addNewItem(newInvItem, true, true);
			newInv->addNewItem(newInvItem, true, true);
			newInvItem.quantity = -39;
			newInv->addNewItem(newInvItem, true, true);

			if (newInv->getItemQuantity(newInvItem.item->uniqueID) != 99 + 60)
			{
				AddError(TEXT("Too much was removed"));
				return false;
			}

			TSubclassOf<AActor> lootBagClass = Cast<UBlueprint>(lootBagObject)->GeneratedClass;
			TArray<AActor*> lootBags;
			UGameplayStatics::GetAllActorsOfClass(World, lootBagClass, lootBags);

			if (lootBags.Num() != 0)
			{
				AddError(TEXT("Item was incorrectly dropped"));
				return false;
			}

			return true;
		}
	}
}

//Testing to make sure you cant add more rows than the max
bool AddRowsTest::RunTest(const FString& Parameters)
{
	FAutomationEditorCommonUtils::CreateNewMap();
	UWorld* World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();

	FStringAssetReference chestAssetPath("/Game/Blueprints/Interactables/Placeables/Chest.Chest");
	UObject* chestObject = chestAssetPath.TryLoad();
	if (!IsValid(chestObject))
	{
		AddError(TEXT("Could not load chest asset"));
		return false;
	}

	FInvItem newInvItem;
	newInvItem.quantity = 99;

	FStringAssetReference itemAssetPath("/Game/Blueprints/Interactables/Harvestables/HarvestableDataAssets/Wood/Sticks.Sticks");
	UObject* itemObject = itemAssetPath.TryLoad();
	UItemAsset* itemAsset = Cast<UItemAsset>(itemObject);
	if (!IsValid(itemAsset))
	{
		AddError(TEXT("Could not load inv item"));
		return false;
	}
	newInvItem.item = itemAsset;

	{
		AActor* newActor = World->SpawnActor<AActor>(Cast<UBlueprint>(chestObject)->GeneratedClass, FVector(0, 0, 0), UKismetMathLibrary::MakeRotator(0, 0, 0));
		if (!IsValid(newActor))
		{
			AddError(TEXT("Actor didn't spawn"));
			return false;
		}
		else
		{
			UInventoryComponent* newInv = Cast<UInventoryComponent>(newActor->GetComponentByClass(UInventoryComponent::StaticClass()));

			bool addOneRow = newInv->addNewRows(1, true);
			bool addRowNoMaterial = newInv->addNewRows(1, false);

			newInv->addNewItem(newInvItem);
			bool addRowMaterial = newInv->addNewRows(1, false);
			int upgradeMatQuant = newInv->getItemQuantity(newInvItem.item->uniqueID);

			bool addSevenRows = newInv->addNewRows(7, true);

			if (!addOneRow)
			{
				AddError(TEXT("Add one row failed"));
			}

			if (addRowNoMaterial)
			{
				AddError(TEXT("Incorrectly added row when there was no material"));
			}

			if (!addRowMaterial)
			{
				AddError(TEXT("Could not add row when it had the material"));
			}

			if (upgradeMatQuant != 96)
			{
				AddError(TEXT("Used the incorrect amount of materials"));
			}

			if (!addSevenRows)
			{
				AddError(TEXT("Could not add the rest of the rows to max"));
			}

			if (newInv->getRows() != 5)
			{
				AddError(TEXT("Did not end with the max amount of rows"));
			}
			
			
			if (FAutomationTestBase::HasAnyErrors())
			{
				return false;
			}
			else
			{
				return true;
			}
		}
	}
}