#if !UE_BUILD_SHIPPING

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationEditorCommon.h"
#include "InventoryComponent.h"
#include "EngineUtils.h" 
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(AddItemEmptyTest, "Inventory.Add Empty", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(AddItemFullTest, "Inventory.Add Full Drop", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(AddItemPartialTest, "Inventory.Add Drop Partially", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(AddAndGetItemAtSlotTest, "Inventory.Add & get Item at slot", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(RemoveItemAtSlotTest, "Inventory.Add & get Item at slot", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(RemoveItemEmptyTest, "Inventory.Remove Item Empty", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(RemoveItemFullStackTest, "Inventory.Remove Full Stack", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(RemoveItemPartialStackTest, "Inventory.Remove Partial Stack", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(AddRowsTest, "Inventory.Add New Rows", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(MoveToOtherInvTest, "Inventory.Move to another Inventory", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(LoadInvTest, "Inventory.Loading an Inventory", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

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

	FSoftObjectPath itemAssetPath("/Game/Blueprints/Plants/DataAssets/Seeds/WheatSeed.WheatSeed");
	UObject* itemObject = itemAssetPath.TryLoad();
	UItemAsset* itemAsset = Cast<UItemAsset>(itemObject);
	if (!IsValid(itemAsset))
	{
		AddError(TEXT("Could not load inv item"));
		return false;
	}
	newInvItem.item = itemAsset;
	
	FSoftObjectPath chestAssetPath("/Game/Blueprints/Interactables/Placeables/Chest.Chest");
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
	FAutomationEditorCommonUtils::LoadMap("/Game/Maps/AutomatedTestsMap.AutomatedTestsMap'");
	UWorld* World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();//FAutomationEditorCommonUtils::CreateNewMap();

	FInvItem newInvItem;
	newInvItem.quantity = 99;

	FSoftObjectPath itemAssetPath("/Game/Blueprints/Plants/DataAssets/Seeds/WheatSeed.WheatSeed");
	UObject* itemObject = itemAssetPath.TryLoad();
	UItemAsset* itemAsset = Cast<UItemAsset>(itemObject);
	if (!IsValid(itemAsset))
	{
		AddError(TEXT("Could not load inv item"));
		return false;
	}
	newInvItem.item = itemAsset;

	FSoftObjectPath chestAssetPath("/Game/Blueprints/Interactables/Placeables/Chest.Chest");
	UObject* chestObject = chestAssetPath.TryLoad();
	if (!IsValid(chestObject))
	{
		AddError(TEXT("Could not load chest asset"));
		return false;
	}
	
	FSoftObjectPath lootBagAssetPath("/Game/Blueprints/Interactables/LootBag.LootBag");
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
	FAutomationEditorCommonUtils::LoadMap("/Game/Maps/AutomatedTestsMap.AutomatedTestsMap'");
	UWorld* World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();//FAutomationEditorCommonUtils::CreateNewMap();

	FInvItem newInvItem;
	newInvItem.quantity = 99;

	FSoftObjectPath itemAssetPath("/Game/Blueprints/Plants/DataAssets/Seeds/WheatSeed.WheatSeed");
	UObject* itemObject = itemAssetPath.TryLoad();
	UItemAsset* itemAsset = Cast<UItemAsset>(itemObject);
	if (!IsValid(itemAsset))
	{
		AddError(TEXT("Could not load inv item"));
		return false;
	}
	newInvItem.item = itemAsset;

	FSoftObjectPath chestAssetPath("/Game/Blueprints/Interactables/Placeables/Chest.Chest");
	UObject* chestObject = chestAssetPath.TryLoad();
	if (!IsValid(chestObject))
	{
		AddError(TEXT("Could not load chest asset"));
		return false;
	}

	FSoftObjectPath lootBagAssetPath("/Game/Blueprints/Interactables/LootBag.LootBag");
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


bool AddAndGetItemAtSlotTest::RunTest(const FString& Parameters)
{
	FAutomationEditorCommonUtils::CreateNewMap();
	FAutomationEditorCommonUtils::LoadMap("/Game/Maps/AutomatedTestsMap.AutomatedTestsMap'");
	UWorld* World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();

	FInvItem newInvItem;
	newInvItem.quantity = 99;

	FSoftObjectPath itemAssetPath("/Game/Blueprints/Plants/DataAssets/Seeds/WheatSeed.WheatSeed");
	UObject* itemObject = itemAssetPath.TryLoad();
	UItemAsset* itemAsset = Cast<UItemAsset>(itemObject);
	if (!IsValid(itemAsset))
	{
		AddError(TEXT("Could not load inv item"));
		return false;
	}
	newInvItem.item = itemAsset;

	FSoftObjectPath chestAssetPath("/Game/Blueprints/Interactables/Placeables/Chest.Chest");
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

		UInventoryComponent* newInv = Cast<UInventoryComponent>(newActor->GetComponentByClass(UInventoryComponent::StaticClass()));
		newInv->addItemAtSlot(newInvItem, 1);
		newInv->addItemAtSlot(newInvItem, 4);
		if (newInv->getItemQuantity(newInvItem.item->uniqueID) != 99 * 2)
		{
			AddError(TEXT("Items did not fully add"));
			return false;
		}

		FInvItem slot0 = newInv->getItemAtSlot(0);
		FInvItem slot1 = newInv->getItemAtSlot(1);
		FInvItem slot2 = newInv->getItemAtSlot(2);
		FInvItem slot3 = newInv->getItemAtSlot(3);
		FInvItem slot4 = newInv->getItemAtSlot(4);

		if (slot1.item == nullptr || slot4.item == nullptr)
		{
			AddError(TEXT("Correct slots are empty"));
		}

		if(slot0.item != nullptr || slot2.item != nullptr || slot3.item != nullptr)
		{
			AddError(TEXT("Slot that should be empty has an item in it"));
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

//Trying to remove an item that isnt in the inventory
bool RemoveItemEmptyTest::RunTest(const FString& Parameters)
{
	FAutomationEditorCommonUtils::CreateNewMap();
	UWorld* World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();

	FInvItem newInvItem;
	newInvItem.quantity = -99;

	FSoftObjectPath itemAssetPath("/Game/Blueprints/Plants/DataAssets/Seeds/WheatSeed.WheatSeed");
	UObject* itemObject = itemAssetPath.TryLoad();
	UItemAsset* itemAsset = Cast<UItemAsset>(itemObject);
	if (!IsValid(itemAsset))
	{
		AddError(TEXT("Could not load inv item"));
		return false;
	}
	newInvItem.item = itemAsset;

	FSoftObjectPath chestAssetPath("/Game/Blueprints/Interactables/Placeables/Chest.Chest");
	UObject* chestObject = chestAssetPath.TryLoad();
	if (!IsValid(chestObject))
	{
		AddError(TEXT("Could not load chest asset"));
		return false;
	}

	FSoftObjectPath lootBagAssetPath("/Game/Blueprints/Interactables/LootBag.LootBag");
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

	FSoftObjectPath itemAssetPath("/Game/Blueprints/Plants/DataAssets/Seeds/WheatSeed.WheatSeed");
	UObject* itemObject = itemAssetPath.TryLoad();
	UItemAsset* itemAsset = Cast<UItemAsset>(itemObject);
	if (!IsValid(itemAsset))
	{
		AddError(TEXT("Could not load inv item"));
		return false;
	}
	newInvItem.item = itemAsset;

	FSoftObjectPath chestAssetPath("/Game/Blueprints/Interactables/Placeables/Chest.Chest");
	UObject* chestObject = chestAssetPath.TryLoad();
	if (!IsValid(chestObject))
	{
		AddError(TEXT("Could not load chest asset"));
		return false;
	}

	FSoftObjectPath lootBagAssetPath("/Game/Blueprints/Interactables/LootBag.LootBag");
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

	FSoftObjectPath itemAssetPath("/Game/Blueprints/Plants/DataAssets/Seeds/WheatSeed.WheatSeed");
	UObject* itemObject = itemAssetPath.TryLoad();
	UItemAsset* itemAsset = Cast<UItemAsset>(itemObject);
	if (!IsValid(itemAsset))
	{
		AddError(TEXT("Could not load inv item"));
		return false;
	}
	newInvItem.item = itemAsset;

	FSoftObjectPath chestAssetPath("/Game/Blueprints/Interactables/Placeables/Chest.Chest");
	UObject* chestObject = chestAssetPath.TryLoad();
	if (!IsValid(chestObject))
	{
		AddError(TEXT("Could not load chest asset"));
		return false;
	}

	FSoftObjectPath lootBagAssetPath("/Game/Blueprints/Interactables/LootBag.LootBag");
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

	FSoftObjectPath chestAssetPath("/Game/Blueprints/Interactables/Placeables/Chest.Chest");
	UObject* chestObject = chestAssetPath.TryLoad();
	if (!IsValid(chestObject))
	{
		AddError(TEXT("Could not load chest asset"));
		return false;
	}

	FInvItem newInvItem;
	newInvItem.quantity = 99;

	FSoftObjectPath itemAssetPath("/Game/Blueprints/Interactables/Harvestables/HarvestableDataAssets/Wood/Sticks.Sticks");
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



//Removing an item at a specific slot
bool RemoveItemAtSlotTest::RunTest(const FString& Parameters)
{
	FAutomationEditorCommonUtils::CreateNewMap();
	FAutomationEditorCommonUtils::LoadMap("/Game/Maps/AutomatedTestsMap.AutomatedTestsMap'");
	UWorld* World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();

	FInvItem newInvItem;
	newInvItem.quantity = 99;

	FSoftObjectPath itemAssetPath("/Game/Blueprints/Plants/DataAssets/Seeds/WheatSeed.WheatSeed");
	UObject* itemObject = itemAssetPath.TryLoad();
	UItemAsset* itemAsset = Cast<UItemAsset>(itemObject);
	if (!IsValid(itemAsset))
	{
		AddError(TEXT("Could not load inv item"));
		return false;
	}
	newInvItem.item = itemAsset;

	FSoftObjectPath chestAssetPath("/Game/Blueprints/Interactables/Placeables/Chest.Chest");
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

		UInventoryComponent* newInv = Cast<UInventoryComponent>(newActor->GetComponentByClass(UInventoryComponent::StaticClass()));
		newInv->addItemAtSlot(newInvItem, 1);
		newInv->addItemAtSlot(newInvItem, 4);
		if (newInv->getItemQuantity(newInvItem.item->uniqueID) != 99 * 2)
		{
			AddError(TEXT("Items did not fully add"));
			return false;
		}

		newInv->removeItem(4);
		FInvItem slot0 = newInv->getItemAtSlot(0);
		FInvItem slot1 = newInv->getItemAtSlot(1);
		FInvItem slot2 = newInv->getItemAtSlot(2);
		FInvItem slot3 = newInv->getItemAtSlot(3);
		FInvItem slot4 = newInv->getItemAtSlot(4);

		if (slot1.item == nullptr)
		{
			AddError(TEXT("Correct slots are empty"));
		}

		if (slot0.item != nullptr || slot2.item != nullptr || slot3.item != nullptr || slot4.item != nullptr)
		{
			AddError(TEXT("Slot that should be empty has an item in it"));
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

bool MoveToOtherInvTest::RunTest(const FString& Parameters)
{
	FAutomationEditorCommonUtils::CreateNewMap();
	FAutomationEditorCommonUtils::LoadMap("/Game/Maps/AutomatedTestsMap.AutomatedTestsMap'");
	UWorld* World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();

	FInvItem newInvItem;
	newInvItem.quantity = 99;

	FSoftObjectPath itemAssetPath("/Game/Blueprints/Plants/DataAssets/Seeds/WheatSeed.WheatSeed");
	UObject* itemObject = itemAssetPath.TryLoad();
	UItemAsset* itemAsset = Cast<UItemAsset>(itemObject);
	if (!IsValid(itemAsset))
	{
		AddError(TEXT("Could not load inv item"));
		return false;
	}
	newInvItem.item = itemAsset;

	FSoftObjectPath chestAssetPath("/Game/Blueprints/Interactables/Placeables/Chest.Chest");
	UObject* chestObject = chestAssetPath.TryLoad();
	if (!IsValid(chestObject))
	{
		AddError(TEXT("Could not load chest asset"));
		return false;
	}

	{
		AActor* newActor = World->SpawnActor<AActor>(Cast<UBlueprint>(chestObject)->GeneratedClass, FVector(0, 0, 0), UKismetMathLibrary::MakeRotator(0, 0, 0));
		AActor* chestTwo = World->SpawnActor<AActor>(Cast<UBlueprint>(chestObject)->GeneratedClass, FVector(0, 0, 0), UKismetMathLibrary::MakeRotator(0, 0, 0));
		if (!IsValid(newActor) || !IsValid(chestTwo))
		{
			AddError(TEXT("Actors didn't spawn"));
			return false;
		}

		UInventoryComponent* newInv = Cast<UInventoryComponent>(newActor->GetComponentByClass(UInventoryComponent::StaticClass()));
		UInventoryComponent* otherInv = Cast<UInventoryComponent>(chestTwo->GetComponentByClass(UInventoryComponent::StaticClass()));

		newInv->addNewItem(newInvItem);
		newInv->addNewItem(newInvItem);
		if (newInv->getItemQuantity(newInvItem.item->uniqueID) != 99 * 2)
		{
			AddError(TEXT("Items did not fully add"));
			return false;
		}

		newInv->moveToNewInvComp(0, otherInv);

		if (newInv->getItemQuantity(newInvItem.item->uniqueID) != 99)
		{
			AddError(TEXT("Main chest has wrong amount of item"));
		}

		if (otherInv->getItemQuantity(newInvItem.item->uniqueID) != 99)
		{
			AddError(TEXT("Second chest has wrong amount of item"));
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


bool LoadInvTest::RunTest(const FString& Parameters)
{
	FAutomationEditorCommonUtils::CreateNewMap();
	FAutomationEditorCommonUtils::LoadMap("/Game/Maps/AutomatedTestsMap.AutomatedTestsMap'");
	UWorld* World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();

	FInvItem newInvItem;
	newInvItem.quantity = 99;

	FSoftObjectPath itemAssetPath("/Game/Blueprints/Plants/DataAssets/Seeds/WheatSeed.WheatSeed");
	UObject* itemObject = itemAssetPath.TryLoad();
	UItemAsset* itemAsset = Cast<UItemAsset>(itemObject);
	if (!IsValid(itemAsset))
	{
		AddError(TEXT("Could not load inv item"));
		return false;
	}
	newInvItem.item = itemAsset;

	FSoftObjectPath chestAssetPath("/Game/Blueprints/Interactables/Placeables/Chest.Chest");
	UObject* chestObject = chestAssetPath.TryLoad();
	if (!IsValid(chestObject))
	{
		AddError(TEXT("Could not load chest asset"));
		return false;
	}

	{
		AActor* newActor = World->SpawnActor<AActor>(Cast<UBlueprint>(chestObject)->GeneratedClass, FVector(0, 0, 0), UKismetMathLibrary::MakeRotator(0, 0, 0));
		AActor* chestTwo = World->SpawnActor<AActor>(Cast<UBlueprint>(chestObject)->GeneratedClass, FVector(0, 0, 0), UKismetMathLibrary::MakeRotator(0, 0, 0));
		if (!IsValid(newActor) || !IsValid(chestTwo))
		{
			AddError(TEXT("Actors didn't spawn"));
			return false;
		}

		UInventoryComponent* newInv = Cast<UInventoryComponent>(newActor->GetComponentByClass(UInventoryComponent::StaticClass()));

		newInv->addNewItem(newInvItem);
		newInv->addNewItem(newInvItem);
		if (newInv->getItemQuantity(newInvItem.item->uniqueID) != 99 * 2)
		{
			AddError(TEXT("Items did not fully add"));
			return false;
		}

		TArray<FInvItem> savedInv = newInv->getInventory();
		newInv->removeItem(1);
		newInv->loadInventory(savedInv);

		if (newInv->getItemQuantity(newInvItem.item->uniqueID) != 99 * 2)
		{
			AddError(TEXT("Inventory did not correctly reload"));
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

#endif

