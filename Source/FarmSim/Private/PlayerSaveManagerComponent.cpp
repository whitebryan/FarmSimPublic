// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSaveManagerComponent.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "Harvestable.h"
#include "GrowthPlot.h"
#include "DestroyablePiece.h"
#include "ConstructableBuilding.h"
#include "BaseAICharacter.h"
#include "QuestComponent.h"
#include "HarvestablePlacer.h"
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

	saveFinished.BindUObject(this, &UPlayerSaveManagerComponent::passDelegateToBP);
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
			playerSaveGame->numRows = curPlayer->myInventoryComp->getRows() - 1;
		}

		bool chestsSaved = saveChests();
		bool actorsSaved = saveActors();

		savePlayerQuestTags();
		playerSaveGame->autoSaveTime = autoSaveTime;
		playerSaveGame->knownRecipes = learnedRecipes;
		playerSaveGame->currentTools = curPlayer->currentTools;
		playerSaveGame->brokenPieces = brokenPieces;
		playerSaveGame->constructedBuildings = constructedBuildings;
		playerSaveGame->fishStats = fishStats;
		playerSaveGame->discoveredItems = discoveredItems;
		playerSaveGame->playerClothes = playerClothes;
		playerSaveGame->skinColor = skinColor;

		bool harvestablesSaved = false;
		harvestablesSaved = savePlacedHarvestables();
		while (!harvestablesSaved)
		{
			//do nothing till all saves are done
			UKismetSystemLibrary::PrintWarning("Saving");
		}

		UGameplayStatics::AsyncSaveGameToSlot(playerSaveGame, slot, 0, saveFinished);
	}
}



//Calls all individual load functions to load from file
void UPlayerSaveManagerComponent::LoadGame(const FString& slot)
{
	if (UGameplayStatics::DoesSaveGameExist(slot, 0))
	{
		playerSaveGame = Cast<UPlayerSaveGame>(UGameplayStatics::LoadGameFromSlot("Slot1", 0));

		AFarmSimCharacter* curPlayer = Cast<AFarmSimCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		//Loading palyer inventory
		if (IsValid(curPlayer))
		{
			curPlayer->myInventoryComp->addNewRows(playerSaveGame->numRows, true);
			curPlayer->myInventoryComp->loadInventory(playerSaveGame->playerInventory);
		}

		loadChests();
		loadActors();

		if (playerSaveGame->harvestables.Num() > 0)
		{
			loadPlacedHarvestables();
		}

		autoSaveTime = playerSaveGame->autoSaveTime;

		if (playerSaveGame->knownRecipes.Num() > 0)
		{
			learnedRecipes = playerSaveGame->knownRecipes;
		}

		constructedBuildings = playerSaveGame->constructedBuildings;
		brokenPieces = playerSaveGame->brokenPieces;
		fishStats = playerSaveGame->fishStats;
		discoveredItems = playerSaveGame->discoveredItems;
		playerClothes = playerSaveGame->playerClothes;
		skinColor = playerSaveGame->skinColor;


		TArray<TEnumAsByte<ClothesCategory>> keys;
		playerClothes.GetKeys(keys);
		for (TEnumAsByte<ClothesCategory> category : keys)
		{
			curPlayer->changeClothingPiece(playerClothes[category].clothesAsset, playerClothes[category].pieceColor);
		}

		if (playerSaveGame->currentTools.Num() > 0)
		{
			curPlayer->currentTools = playerSaveGame->currentTools;
		}

		//Rebuilding buildings
		TArray<AActor*> foundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AConstructableBuilding::StaticClass(), foundActors);
		for (AActor* curActor : foundActors)
		{
			AConstructableBuilding* curBuilding = Cast<AConstructableBuilding>(curActor);

			if (IsValid(curBuilding) && constructedBuildings.Contains(curBuilding->buildingRecipe->name))
			{
				curBuilding->forceBuild();
			}
		}


		//Rebreaking pieces
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADestroyablePiece::StaticClass(), foundActors);

		for (FVector curLoc : brokenPieces)
		{
			for (AActor* curActor : foundActors)
			{
				if (brokenPieces.Contains(curActor->GetActorLocation()))
				{
					Cast<ADestroyablePiece>(curActor)->breakPiece();
				}
			}
		}

		//Reloading quest status
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), "WorldManager", foundActors);
		if (foundActors.Num() > 0 && playerSaveGame->playerQuests.Num() > 0)
		{
			UQuestTrackerComponent* worldManagerQuestTracker = Cast<UQuestTrackerComponent>(foundActors[0]->GetComponentByClass(UQuestTrackerComponent::StaticClass()));
			worldManagerQuestTracker->loadQuests(playerSaveGame->playerQuests);
		}

		//Reloading npc cur convos
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseAICharacter::StaticClass(), foundActors);
		for (AActor* curActor : foundActors)
		{
			ABaseAICharacter* curNPC = Cast<ABaseAICharacter>(curActor);
			if (IsValid(curNPC))
			{
				if (playerSaveGame->npcConversations.Contains(curNPC->npcName))
				{
					curNPC->loadCurConvo(playerSaveGame->npcConversations[curNPC->npcName]);
				}
			}
		}

		//Readding quest specific tags
		for (int i = 0; i < playerSaveGame->playerTags.Num(); ++i)
		{
			curPlayer->playerTags.AddTag(playerSaveGame->playerTags.GetByIndex(i));
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

void UPlayerSaveManagerComponent::saveSeasonTimeWeather(const FName curWeather, float time, float season, int daysIntoSeason, int dayOfTheWeek)
{
	playerSaveGame->lastWeather = curWeather;
	playerSaveGame->timeOfDay = time;
	playerSaveGame->season = season;
	playerSaveGame->daysIntoSeason = daysIntoSeason;
	playerSaveGame->dayOfTheWeek = dayOfTheWeek;
}



//Goes through all actors with BaseChest tag and saves their locations and inventories
bool UPlayerSaveManagerComponent::saveChests()
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

	return true;
}

//Spawns in all previously placed chests and readds their inventories
void UPlayerSaveManagerComponent::loadChests()
{
	for (int i = 0; i < playerSaveGame->placedChests.Num(); ++i)
	{
		AActor* newChest = GetWorld()->SpawnActor<AActor>(chestClass,playerSaveGame->placedChests[i].chestTransform);
		UInventoryComponent* newChestInv = Cast<UInventoryComponent>(newChest->GetComponentByClass(UInventoryComponent::StaticClass()));
		newChestInv->addNewRows(playerSaveGame->placedChests[i].numRows, true);
		newChestInv->loadInventory(playerSaveGame->placedChests[i].chestInventory);
	}
}

bool UPlayerSaveManagerComponent::saveActors()
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

	return true;
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
}

void UPlayerSaveManagerComponent::saveDLSSMode(UDLSSMode newMode)
{
	playerSaveGame->playerDLSSMode = newMode;
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

void UPlayerSaveManagerComponent::passDelegateToBP(const FString& saveString, const int32 saveInt, bool saveDone)
{
	bpSaveFinishedDelegate.Broadcast();
}

void UPlayerSaveManagerComponent::addClothingPiece(UModularClothingAsset* pieceToAdd, FLinearColor color)
{
	FClothesSaveStruct newClothesStruct;
	newClothesStruct.clothesAsset = pieceToAdd;
	newClothesStruct.pieceColor = color;

	playerClothes.Add(pieceToAdd->category, newClothesStruct);
}


void UPlayerSaveManagerComponent::saveNPCConversationStatus(FString npcName, int curConvo)
{
	playerSaveGame->npcConversations.Add(npcName, curConvo);
}


void UPlayerSaveManagerComponent::saveQuestStatus(FQuest quest)
{
	playerSaveGame->playerQuests.Add(quest.Quest->questName, quest);
	saveGame();
}

void UPlayerSaveManagerComponent::savePlayerQuestTags()
{
	AFarmSimCharacter* curPlayer = Cast<AFarmSimCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	for (int i = 0; i < curPlayer->playerTags.Num(); ++i)
	{
		FGameplayTag curTag = curPlayer->playerTags.GetByIndex(i);
		if (curTag.GetTagName().ToString().Contains("Quest"))
		{
			playerSaveGame->playerTags.AddTag(curTag);
		}
	}
}

bool UPlayerSaveManagerComponent::savePlacedHarvestables()
{
	playerSaveGame->harvestables.Empty();

	TArray<AActor*> harvestablePlacers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHarvestablePlacer::StaticClass(), harvestablePlacers);

	for (int i = 0; i < harvestablePlacers.Num(); ++i)
	{
		FHarvestableSaveStruct newActor = Cast<AHarvestablePlacer>(harvestablePlacers[i])->generateSpawnArray();
		newActor.actorType = nullptr;
		newActor.actorTransform = harvestablePlacers[i]->GetTransform();
		newActor.actorTimer = 0;

		playerSaveGame->harvestables.Add(harvestablePlacers[i]->GetActorLocation(), newActor);
	}
	return true;
}

void UPlayerSaveManagerComponent::loadPlacedHarvestables()
{
	TArray<AActor*> harvestablePlacers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHarvestablePlacer::StaticClass(), harvestablePlacers);

	for (int i = 0; i < harvestablePlacers.Num(); ++i)
	{
		if (playerSaveGame->harvestables.Contains(harvestablePlacers[i]->GetActorLocation()))
		{
			Cast<AHarvestablePlacer>(harvestablePlacers[i])->reloadHarvestables(playerSaveGame->harvestables[harvestablePlacers[i]->GetActorLocation()]);
		}
	}
}