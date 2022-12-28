// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSaveManagerComponent.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"


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
	if (!UGameplayStatics::DoesSaveGameExist("Slot1", 0))
	{
		playerSaveGame = Cast<UPlayerSaveGame>(UGameplayStatics::CreateSaveGameObject(UPlayerSaveGame::StaticClass()));
	}
	else
	{
		playerSaveGame = Cast<UPlayerSaveGame>(UGameplayStatics::LoadGameFromSlot("Slot1", 0));
	}
	// ...
	
}

// Called every frame
void UPlayerSaveManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPlayerSaveManagerComponent::saveGame(FString slot)
{
	if (playerSaveGame)
	{
		UGameplayStatics::AsyncSaveGameToSlot(playerSaveGame, slot, 0, saveFinished);
	}
}

void UPlayerSaveManagerComponent::loadGame(FString slot)
{
	if (UGameplayStatics::DoesSaveGameExist(slot, 0))
	{
		playerSaveGame = Cast<UPlayerSaveGame>(UGameplayStatics::LoadGameFromSlot("Slot1", 0));
		loadFinished.Broadcast(true);
	}
	else
	{
		loadFinished.Broadcast(false);
	}
}

void UPlayerSaveManagerComponent::saveKeybinds(FString slot)
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
		saveGame();
	}
}

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