// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h" 
#include "GameplayTagsModule.h"
#include "../InventoryAndCrafting/InventoryComponent.h"
#include "../Player/FarmSimCharacter.h"
#include "GameplayTagAssetInterface.h" 

UQuestTrackerComponent::UQuestTrackerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UQuestTrackerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UQuestTrackerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UQuestTrackerComponent::startQuest(const FQuest& newQuest)
{
	quests.Add(newQuest.Quest->questName, newQuest);

	AFarmSimCharacter* player = Cast<AFarmSimCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	UPlayerSaveManagerComponent* playerSaveManager = Cast<UPlayerSaveManagerComponent>(player->GetComponentByClass(UPlayerSaveManagerComponent::StaticClass()));

	playerSaveManager->saveQuestStatus(newQuest);
}

FQuestStepStruct UQuestTrackerComponent::checkRemainingProgress(const FString questName)
{
	FQuestStepStruct result;
	if (!quests.Contains(questName))
	{
		UKismetSystemLibrary::PrintWarning("Quest name invalid");
		return result;
	}
	else if (quests[questName].curStep > quests[questName].Quest->questSteps.Num())
	{
		UKismetSystemLibrary::PrintWarning("Already completed");
		return result;
	}

	//Grab the quest and make sure its not already done
	result = quests[questName].Quest->questSteps[quests[questName].curStep];

	AActor* player = Cast<AActor>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	
	//Only check tags if there is one of either type
	if (result.requiredTags.Num() > 0 || result.restrictedTags.Num() > 0)
	{
		//Grab the player tag interface
		FGameplayTagContainer playerTags;
		IGameplayTagAssetInterface* playerTagInterface = Cast<IGameplayTagAssetInterface>(player);
		if (playerTagInterface != nullptr)
		{
			playerTagInterface->GetOwnedGameplayTags(playerTags);
			FGameplayTagContainer tagsToRemove;

			//Remove tags the player has that are needed
			if (result.requiredTags.Num() > 0)
			{
				for (int i = 0; i < result.requiredTags.Num(); ++i)
				{
					FGameplayTag curTag = result.requiredTags.GetByIndex(i);
					
					if (curTag.MatchesAnyExact(playerTags))
					{
						tagsToRemove.AddTag(curTag);
					}
				}
				if (result.requiredTags.Num() > 0 && tagsToRemove.Num() > 0)
				{
					result.requiredTags.RemoveTags(tagsToRemove);
				}

				tagsToRemove.Reset();
			}

			//Removed tags the player doesn't have that would restrict completion
			if (result.restrictedTags.Num() > 0)
			{
				for (int i = 0; i < result.restrictedTags.Num(); ++i)
				{
					FGameplayTag curTag = result.restrictedTags.GetByIndex(i);
					if (!curTag.MatchesAnyExact(playerTags))
					{
						tagsToRemove.AddTag(curTag);
					}
				}
				if (result.restrictedTags.Num() > 0 && tagsToRemove.Num() > 0)
				{
					result.restrictedTags.RemoveTags(tagsToRemove);
				}
			}
		}
	}

	if (result.turnInItems.Num() > 0)
	{
		UInventoryComponent* playerInv = Cast<UInventoryComponent>(player->GetComponentByClass(UInventoryComponent::StaticClass()));
		if (!IsValid(playerInv))
		{
			UKismetSystemLibrary::PrintWarning("Couldnt get player Inventory");
			return result;
		}

		TArray<FInvItem> tempRequired = result.turnInItems;
		for (int i = 0; i < result.turnInItems.Num(); ++i)
		{
			if (playerInv->getItemQuantity(tempRequired[i].item->uniqueID) >= tempRequired[i].quantity)
			{
				result.turnInItems.RemoveAt(i);
			}
		}
	}

	return result;
}

void UQuestTrackerComponent::turnInQuestStep(const FString questName)
{
	if (!quests.Contains(questName))
	{
		UKismetSystemLibrary::PrintWarning("No Quest");
		return;
	}
	else if (quests[questName].curStep >= quests[questName].Quest->questSteps.Num())
	{
		UKismetSystemLibrary::PrintWarning("Quest completed");
		return;
	}

	FQuestStepStruct checkProgress = checkRemainingProgress(questName);
	if (checkProgress.requiredTags.Num() == 0 && checkProgress.restrictedTags.Num() == 0 && checkProgress.turnInItems.Num() == 0)
	{
		AFarmSimCharacter* player = Cast<AFarmSimCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

		UPlayerSaveManagerComponent* playerSaveManager = Cast<UPlayerSaveManagerComponent>(player->GetComponentByClass(UPlayerSaveManagerComponent::StaticClass()));

		//remove step or quest specific tags
		stepCompleted.Broadcast(quests[questName].Quest->questSteps[quests[questName].curStep]);
		++quests[questName].curStep;
		playerSaveManager->saveQuestStatus(quests[questName]);

		//Display notifications for step or quest overall rewards
		FString notification ="null";
		if (quests[questName].curStep >= quests[questName].Quest->questSteps.Num())
		{
			notification = "Quest " + questName + " completed!";

			if (quests[questName].Quest->rewardItems.Num() > 0)
			{
				notification += '\n' + "Receieved : ";
				for (int i = 0; i < quests[questName].Quest->rewardItems.Num(); ++i)
				{
					notification += FString::FromInt(quests[questName].Quest->rewardItems[i].quantity) + "x" + quests[questName].Quest->rewardItems[i].item->name.ToString() + " ";
				}
			}
			questCompleted.Broadcast(quests[questName].Quest);
		}

		if (quests[questName].Quest->questSteps[quests[questName].curStep-1].rewardItems.Num() > 0)
		{
			if (notification == "null")
			{
				notification = "Quest step " + quests[questName].Quest->questSteps[quests[questName].curStep - 1].stepName + " completed!";
				notification += '\n' + "Receieved : ";
			}

			for (int i = 0; i < quests[questName].Quest->questSteps[quests[questName].curStep - 1].rewardItems.Num(); ++i)
			{
				notification += FString::FromInt(quests[questName].Quest->questSteps[quests[questName].curStep - 1].rewardItems[i].quantity) + "x" + quests[questName].Quest->questSteps[quests[questName].curStep - 1].rewardItems[i].item->name.ToString() + " ";
			}
		}

		if (notification != "null")
		{
			player->displayNotification(notification, 3);
		}
	}
	else
	{
		//make notifcation saying whats needed
		UKismetSystemLibrary::PrintWarning("Not Done");
	}
}

bool UQuestTrackerComponent::checkForCompletedQuest(FString questName)
{
	if (quests.Contains(questName) && quests[questName].curStep >= quests[questName].Quest->questSteps.Num())
	{
		return true;
	}
	else
	{
		return false;
	}
}

const FQuest UQuestTrackerComponent::getQuestStatus(const FString& questName)
{
	if (quests.Contains(questName))
	{
		return quests[questName];
	}
	else
	{
		return FQuest();
	}
}