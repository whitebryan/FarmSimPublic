#if !UE_BUILD_SHIPPING

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationEditorCommon.h"
#include "InventoryComponent.h"
#include "EngineUtils.h" 
#include "Engine/World.h"
#include "CraftingTable.h"
#include "Kismet/KismetMathLibrary.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(GenerateCraftableTest, "Crafting.Generate Craftable Items", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(CraftBasicTest, "Crafting.Craft Basic Item", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(CraftToolTest, "Crafting.Craft Tool", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(CraftPlayerOnlyTest, "Crafting.Craft from player Inventory", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(CraftPlayerAndChestsTest, "Crafting.Craft from player Inventory and chests", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(CraftFullInventoryTest, "Crafting.Craft full inventory", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(CraftBuildingTest, "Crafting.Create Building", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)




bool GenerateCraftableTest::RunTest(const FString& Paramaters)
{
	FAutomationEditorCommonUtils::CreateNewMap();
	FAutomationEditorCommonUtils::LoadMap("/Game/Maps/AutomatedTestsMap.AutomatedTestsMap'");
	FAutomationEditorCommonUtils::RunPIE();
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

bool CraftBasicTest::RunTest(const FString& Paramaters)
{
	//make nails
	// /Game/Blueprints/Recipes/NailsRecipe.NailsRecipe
	// /Game/Blueprints/Interactables/Harvestables/HarvestableDataAssets/Stone/SmallIronChuns.SmallIronChuns need 1 per

	return true;
}

bool CraftToolTest::RunTest(const FString& Paramaters)
{
	//make bronze pickaxe
	// /Game/Blueprints/Recipes/ToolRecipes/BronzePickaxeRecipe.BronzePickaxeRecipe
	// /Game/Blueprints/Interactables/Harvestables/HarvestableDataAssets/Stone/Rocks.Rocks 3
	// /Game/Blueprints/Interactables/Harvestables/HarvestableDataAssets/Wood/Sticks.Sticks' 2

	return true;
}

bool CraftPlayerOnlyTest::RunTest(const FString& Paramaters)
{
	return true;
}

bool CraftPlayerAndChestsTest::RunTest(const FString& Paramaters)
{
	// /Game/Blueprints/Recipes/WoodenPlankRecipe.WoodenPlankRecipe
	// /Game/Blueprints/Interactables/Harvestables/HarvestableDataAssets/Wood/LowQualityWood.LowQualityWood 3

	return true;
}

bool CraftFullInventoryTest::RunTest(const FString& Paramaters)
{
	// /Game/Blueprints/Recipes/WoodenPlankRecipe.WoodenPlankRecipe
	// /Game/Blueprints/Interactables/Harvestables/HarvestableDataAssets/Wood/LowQualityWood.LowQualityWood 3

	return true;
}

bool CraftBuildingTest::RunTest(const FString& Paramaters)
{
	// /Game/Blueprints/Recipes/QuestRecipes/TutorialBridgeRecipe1.TutorialBridgeRecipe1
	// /Game/Blueprints/QuestItemAssets/BridgeConcrete.BridgeConcrete 1

	return true;
}

#endif