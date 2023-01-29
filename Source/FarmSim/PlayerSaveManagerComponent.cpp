// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSaveManagerComponent.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "Harvestable.h"
#include "GrowthPlot.h"
#include "DestroyablePiece.h"
#include "ConstructableBuilding.h"
#include "FarmSimCharacter.h"


// Sets default values for this component's properties
UPlayerSaveManagerComponent::UPlayerSaveManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	// ...
}


// Called when the game starts
void UPlayerSaveManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	//Load save if it exists otherwise make it
	if (!UGameplayStatics::DoesSaveGameExist("Slot1", 0))
	{
		playerSaveGame = Cast<UPlayerSaveGame>(UGameplayStatics::CreateSaveGameObject(UPlayerSaveGame::StaticClass()));
	}
	else
	{
		playerSaveGame = Cast<UPlayerSaveGame>(UGameplayStatics::LoadGameFromSlot("Slot1", 0));
	}

	FTimerDelegate autoSaveDelegate;
	FString slotN = "Slot1";
	autoSaveDelegate.BindUFunction(this, FName("saveGame"), slotN);
	GetWorld()->GetTimerManager().SetTimer(autoSaveHandle, autoSaveDelegate, (autoSaveTime * 60), true);
	// ...
}

// Called every frame
void UPlayerSaveManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

//Calls all individual save functions then saves to file
void UPlayerSaveManagerComponent::saveGame(const FString& slot)
{
	if (playerSaveGame)
	{
		//Call the blueprint side in play character to save day/weather/season
		SaveRequested.Broadcast();

		AFarmSimCharacter* curPlayer = Cast<AFarmSimCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

		if (IsValid(curPlayer))
		{
			playerSaveGame->playerInventory = curPlayer->myInventoryComp->getInventory();
			playerSaveGame->numRows = curPlayer->myInventoryComp->getRows();
		}

		saveChests();
		saveActors();
		playerSaveGame->autoSaveTime = autoSaveTime;
		playerSaveGame->knownRecipes = learnedRecipes;
		playerSaveGame->currentTools = curPlayer->currentTools;
		playerSaveGame->brokenPieces = brokenPieces;
		playerSaveGame->constructedBuildings = constructedBuildings;
		playerSaveGame->fishStats = fishStats;
		playerSaveGame->discoveredItems = discoveredItems;
		UGameplayStatics::AsyncSaveGameToSlot(playerSaveGame, slot, 0, saveFinished);
	}
}

//Calls all individual load functions to load from file
void UPlayerSaveManagerComponent::LoadGame(const FString& slot)
{
	if (UGameplayStatics::DoesSaveGameExist(slot, 0))
	{
		AFarmSimCharacter* curPlayer = Cast<AFarmSimCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		//Loading palyer inventory
		if (IsValid(curPlayer))
		{
			curPlayer->myInventoryComp->loadInventory(playerSaveGame->playerInventory, playerSaveGame->numRows - 1);
		}

		loadKeybinds();
		loadChests();
		loadActors();
		autoSaveTime = playerSaveGame->autoSaveTime;
		learnedRecipes = playerSaveGame->knownRecipes;
		constructedBuildings = playerSaveGame->constructedBuildings;
		brokenPieces = playerSaveGame->brokenPieces;
		fishStats = playerSaveGame->fishStats;
		discoveredItems = playerSaveGame->discoveredItems;
		curPlayer->currentTools = playerSaveGame->currentTools;

		//Rebuilding buildings
		TArray<AActor*> buildings;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AConstructableBuilding::StaticClass(), buildings);
		for (AActor* curActor : buildings)
		{
			AConstructableBuilding* curBuilding = Cast<AConstructableBuilding>(curActor);

			if (IsValid(curBuilding) && constructedBuildings.Contains(curBuilding->buildingRecipe->name))
			{
				curBuilding->forceBuild();
			}
		}


		//Rebreaking pieces
		TArray<AActor*> breakablePieces;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADestroyablePiece::StaticClass(), breakablePieces);

		for (FVector curLoc : brokenPieces)
		{
			for (AActor* curActor : breakablePieces)
			{
				if (brokenPieces.Contains(curActor->GetActorLocation()))
				{
					Cast<ADestroyablePiece>(curActor)->breakPiece();
				}
			}
		}

		loadFinished.Broadcast(true);
	}
	else
	{
		loadFinished.Broadcast(false);
	}
}

void UPlayerSaveManagerComponent::changeAutoSaveTimer(int newTime)
{
	if (GetWorld()->GetTimerManager().IsTimerActive(autoSaveHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(autoSaveHandle);

		autoSaveTime = newTime;

		FTimerDelegate autoSaveDelegate;
		FString slotN = "Slot1";
		autoSaveDelegate.BindUFunction(this, FName("saveGame"), slotN);
		GetWorld()->GetTimerManager().SetTimer(autoSaveHandle, autoSaveDelegate, (autoSaveTime * 60), true);
	}
}

void UPlayerSaveManagerComponent::saveSeasonTimeWeather(const FName curWeather, float time, float season, int daysIntoSeason)
{
	playerSaveGame->lastWeather = curWeather;
	playerSaveGame->timeOfDay = time;
	playerSaveGame->season = season;
	playerSaveGame->daysIntoSeason = daysIntoSeason;
}



//Goes through all actors with BaseChest tag and saves their locations and inventories
void UPlayerSaveManagerComponent::saveChests()
{
	playerSaveGame->placedChests.Empty();
	
	TArray<AActor*> actorsFound;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AActor::StaticClass(), "BaseChest", actorsFound);

	for (int i = 0; i < actorsFound.Num(); ++i)
	{
		UInventoryComponent* curChestInvComp = Cast<UInventoryComponent>(actorsFound[i]->GetComponentByClass(UInventoryComponent::StaticClass()));

		FChestSaveStruct newChest;
		newChest.chestInventory = curChestInvComp->getInventory();
		newChest.numRows = curChestInvComp->getRows() - 1;
		newChest.chestTransform = actorsFound[i]->GetTransform();

		playerSaveGame->placedChests.Add(newChest);
	}
}

//Spawns in all previously placed chests and readds their inventories
void UPlayerSaveManagerComponent::loadChests()
{
	for (int i = 0; i < playerSaveGame->placedChests.Num(); ++i)
	{
		AActor* newChest = GetWorld()->SpawnActor<AActor>(chestClass,playerSaveGame->placedChests[i].chestTransform);
		UInventoryComponent* newChestInv = Cast<UInventoryComponent>(newChest->GetComponentByClass(UInventoryComponent::StaticClass()));
		newChestInv->loadInventory(playerSaveGame->placedChests[i].chestInventory, playerSaveGame->placedChests[i].numRows);
	}
}

//Saves all rebindable keybinds
void UPlayerSaveManagerComponent::saveKeybinds(const FString& slot)
{
	playerSaveGame = Cast<UPlayerSaveGame>(UGameplayStatics::CreateSaveGameObject(UPlayerSaveGame::StaticClass()));
	if (playerSaveGame)
	{
		//Getting our inputs marked mappable and going through them saving them to our current savegame
		TArray<FEnhancedActionKeyMapping> playerMappableKeys = defaultConfig->GetPlayerMappableKeys();

		for (FEnhancedActionKeyMapping curMapping : playerMappableKeys)
		{
			playerSaveGame->inputMap.Add(curMapping.PlayerMappableOptions.Name, curMapping.Key);
		}
	}
}

//Reloads all rebindable keybinds
void UPlayerSaveManagerComponent::loadKeybinds()
{
	if (!playerSaveGame)
	{
		return;
	}
	
	//Getting mappable keys and default context
	TArray<FEnhancedActionKeyMapping> playerMappableKeys = defaultConfig->GetPlayerMappableKeys();

	TArray<TObjectPtr<UInputMappingContext>> contextArray;
	TMap<TObjectPtr<UInputMappingContext>, int32> contextMap = defaultConfig->GetMappingContexts();
	contextMap.GetKeys(contextArray);
	TObjectPtr<UInputMappingContext> defaultContext = contextArray[0];

	for (FEnhancedActionKeyMapping curMapping : playerMappableKeys)
	{
		if (playerSaveGame->inputMap.Contains(curMapping.PlayerMappableOptions.Name))
		{
			defaultContext->UnmapKey(curMapping.Action, curMapping.Key);
			FEnhancedActionKeyMapping& newKeyMapping = defaultContext->MapKey(curMapping.Action, *playerSaveGame->inputMap.Find(curMapping.PlayerMappableOptions.Name));

			newKeyMapping.bIsPlayerMappable = true;
			newKeyMapping.Modifiers = curMapping.Modifiers;
			newKeyMapping.Triggers = curMapping.Triggers;
			newKeyMapping.PlayerMappableOptions = curMapping.PlayerMappableOptions;
		}
	}
}

void UPlayerSaveManagerComponent::saveActors()
{
	playerSaveGame->plantedCrops.Empty();
	playerSaveGame->growthPlots.Empty();
	playerSaveGame->harvestables.Empty();

	//Find and save all crops
	TArray<AActor*> actorsFound;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABasePlant::StaticClass(), actorsFound);
	for (int i = 0; i < actorsFound.Num(); ++i)
	{
		FActorSaveStruct newActor;
		newActor.actorType = actorsFound[i]->GetClass();
		newActor.actorTransform = actorsFound[i]->GetTransform();
		newActor.actorTimer = Cast<ABasePlant>(actorsFound[i])->getTotalWateredTime();

		playerSaveGame->plantedCrops.Add(newActor);
	}

	//Find and save all growth plots that aren't in use
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGrowthPlot::StaticClass(), actorsFound); 
	for (int i = 0; i < actorsFound.Num(); ++i)
	{
		if (Cast<AGrowthPlot>(actorsFound[i])->isInUse())
		{
			continue;
		}

		FActorSaveStruct newActor;
		newActor.actorType = actorsFound[i]->GetClass();
		newActor.actorTransform = actorsFound[i]->GetTransform();
		newActor.actorTimer = 0;

		playerSaveGame->growthPlots.Add(newActor);
	}

	//Find and save all harvestables
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHarvestable::StaticClass(), actorsFound);
	for (int i = 0; i < actorsFound.Num(); ++i)
	{
		FActorSaveStruct newActor;
		newActor.actorType = nullptr;
		newActor.actorTransform = actorsFound[i]->GetTransform();
		newActor.actorTimer = Cast<AHarvestable>(actorsFound[i])->getCurTimeTillRespawn();

		if (newActor.actorTimer > 0)
		{
			playerSaveGame->harvestables.Add(newActor);
		}
	}
}

void UPlayerSaveManagerComponent::loadActors()
{
	//Spawn a growth plot and then all plants
	for (int i = 0; i < playerSaveGame->plantedCrops.Num(); ++i)
	{
		AActor* newPlot = GetWorld()->SpawnActor<AActor>(growthPlotClass, playerSaveGame->plantedCrops[i].actorTransform);
		AActor* newPlant = GetWorld()->SpawnActor<AActor>(playerSaveGame->plantedCrops[i].actorType, playerSaveGame->plantedCrops[i].actorTransform);
		Cast<ABasePlant>(newPlant)->InitalizePlant(false, newPlot, playerSaveGame->plantedCrops[i].actorTimer);
	}

	//Spawn all the unused growth plots
	for (int i = 0; i < playerSaveGame->growthPlots.Num(); ++i)
	{
		AActor* newPlot = GetWorld()->SpawnActor<AActor>(growthPlotClass, playerSaveGame->growthPlots[i].actorTransform);
	}


	//Reinit all harvestables that were in progress
	TArray<AActor*> actorsFound;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHarvestable::StaticClass(), actorsFound);
	for (int i = 0; i < playerSaveGame->harvestables.Num(); ++i)
	{
		for (AActor* curAct : actorsFound)
		{
			if (curAct->GetActorTransform().GetLocation() == playerSaveGame->harvestables[i].actorTransform.GetLocation())
			{
				Cast<AHarvestable>(curAct)->initHarvestable(playerSaveGame->harvestables[i].actorTimer);
				break;
			}
		}
	}
}

void UPlayerSaveManagerComponent::saveDLSSMode(UDLSSMode newMode)
{
	playerSaveGame->playerDLSSMode = newMode;
}

//Change individual keybind
bool UPlayerSaveManagerComponent::rebindAction(FName mappingToChange, FKey newKey)
{
	TArray<FEnhancedActionKeyMapping> playerMappableKeys = defaultConfig->GetPlayerMappableKeys();
	TArray<TObjectPtr<UInputMappingContext>> contextArray;
	TMap<TObjectPtr<UInputMappingContext>, int32> contextMap = defaultConfig->GetMappingContexts();
	contextMap.GetKeys(contextArray);
	TObjectPtr<UInputMappingContext> defaultContext = contextArray[0];

	if (checkIfInUse(newKey))
	{
		return false;
	}

	for (FEnhancedActionKeyMapping curMapping : playerMappableKeys)
	{
		if (curMapping.PlayerMappableOptions.Name == mappingToChange)
		{
			defaultContext->UnmapKey(curMapping.Action, curMapping.Key);
			FEnhancedActionKeyMapping& newKeyMapping = defaultContext->MapKey(curMapping.Action, newKey);

			newKeyMapping.bIsPlayerMappable = true;
			newKeyMapping.Modifiers = curMapping.Modifiers;
			newKeyMapping.Triggers = curMapping.Triggers;
			newKeyMapping.PlayerMappableOptions = curMapping.PlayerMappableOptions;
		}
	}
	saveKeybinds();
	return true;
}


//Reset ALL keybinds to default setting
void UPlayerSaveManagerComponent::resetKeyBinds()
{
	TArray<TObjectPtr<UInputMappingContext>> contextArray;
	TMap<TObjectPtr<UInputMappingContext>, int32> contextMap = defaultConfig->GetMappingContexts();
	contextMap.GetKeys(contextArray);
	TObjectPtr<UInputMappingContext> defaultContext = contextArray[0];

	TArray<FEnhancedActionKeyMapping> playerMappableKeys = defaultConfig->GetPlayerMappableKeys();
	TArray<FEnhancedActionKeyMapping> defaultMappings = DefaultInputMappingContext->GetMappings();

	for (FEnhancedActionKeyMapping curMapping : playerMappableKeys)
	{
		FKey newKey = curMapping.Key;

		for (FEnhancedActionKeyMapping defaultKey : defaultMappings)
		{
			if (defaultKey.PlayerMappableOptions.Name == curMapping.PlayerMappableOptions.Name)
			{
				newKey = defaultKey.Key;
				break;
			}
		}

		if (newKey == curMapping.Key)
		{
			continue;
		}
		else
		{
			defaultContext->UnmapKey(curMapping.Action, curMapping.Key);
			FEnhancedActionKeyMapping& newKeyMapping = defaultContext->MapKey(curMapping.Action, newKey);

			newKeyMapping.bIsPlayerMappable = true;
			newKeyMapping.Modifiers = curMapping.Modifiers;
			newKeyMapping.Triggers = curMapping.Triggers;
			newKeyMapping.PlayerMappableOptions = curMapping.PlayerMappableOptions;
		}
	}
	saveKeybinds();
}


//Check if a key is in use in a keybinding
bool UPlayerSaveManagerComponent::checkIfInUse(FKey keyToCheck)
{
	TArray<FEnhancedActionKeyMapping> mappedKeys = defaultConfig->GetPlayerMappableKeys();

	for (FEnhancedActionKeyMapping curMapping : mappedKeys)
	{
		if (curMapping.Key == keyToCheck)
		{
			return true;
		}
	}

	return false;
}

void UPlayerSaveManagerComponent::addRecipe(URecipeAsset* newRecipe)
{
	if (!learnedRecipes.Contains(newRecipe))
	{
		learnedRecipes.Add(newRecipe);
	}
}

void UPlayerSaveManagerComponent::saveBuildingStatus(const FName buildingName)
{
	constructedBuildings.AddUnique(buildingName);
}

void UPlayerSaveManagerComponent::saveBrokenPiece(FVector loc)
{
	brokenPieces.Add(loc);
}

void UPlayerSaveManagerComponent::tryDiscoverItem(UItemAsset* itemToDiscover)
{
	if (discoveredItems.Contains(itemToDiscover))
	{
		return;
	}
	else
	{
		discoveredItems.Add(itemToDiscover);
		
		FString newItemFound = "Discovered new item!\n" + itemToDiscover->name.ToString() + "!";
		Cast<AFarmSimCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))->displayNotification(newItemFound, 3.5f);
		newItemDiscovered.Broadcast();
	}
}

FFishRecordStruct UPlayerSaveManagerComponent::fishCaught(FName uniqueID, float size, FName fishName)
{
	FFishRecordStruct newFishRecord;
	newFishRecord.newSize = size;
	newFishRecord.fishName = fishName;

	if (!fishStats.Contains(uniqueID))
	{
		FFishStatSaveStruct newFish;
		newFish.curMax = size;
		newFish.curMin = size;

		fishStats.Add(uniqueID, newFish);
		newFishRecord.record = FishSizeStatus::NewFish;
		return newFishRecord;
	}
	else
	{
		FFishStatSaveStruct curFish = fishStats[uniqueID];
		
		if (size > curFish.curMax)
		{
			curFish.curMax = size;
			fishStats.Add(uniqueID, curFish);
			newFishRecord.record = FishSizeStatus::NewMax;
			return newFishRecord;
		}
		else if (size < curFish.curMin)
		{
			curFish.curMin = size;
			fishStats.Add(uniqueID,curFish);
			newFishRecord.record = FishSizeStatus::NewMin;
			return newFishRecord;
		}
		else
		{
			newFishRecord.record = FishSizeStatus::NoChange;
			return newFishRecord;
		}
	}
}
