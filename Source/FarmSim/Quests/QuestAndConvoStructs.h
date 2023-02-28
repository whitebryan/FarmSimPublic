// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "../InventoryAndCrafting/InventoryItem.h"
#include "QuestAndConvoStructs.generated.h"


USTRUCT(BlueprintType, Blueprintable)
struct FQuestStepStruct
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip = "Step name"))
	FString stepName = "None";
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip = "Step description"))
	FString stepDescription = "None";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip = "Items required to finish"))
	TArray<FInvItem> turnInItems;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip = "Items given as reward for step completion"))
	TArray<FInvItem> rewardItems;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip = "Tags added for completing this step"))
	FGameplayTagContainer completionTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip = "Tags required for step completion"))
	FGameplayTagContainer requiredTags;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip = "Tag that restrict step completion"))
	FGameplayTagContainer restrictedTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip = "Dialogue for when this step is completed"))
	UConversationAsset* stepCompleted = nullptr;
};

UCLASS(BlueprintType)
class FARMSIM_API UQuestAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip = "Quest name"))
	FString questName = "None";
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip = "Quest description"))
	FString questDescription = "None";
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip = "Array of steps to quest completion"))
	TArray<FQuestStepStruct> questSteps;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip = "Items given as reward for completeing all steps"))
	TArray<FInvItem> rewardItems;
};


USTRUCT(BlueprintType, Blueprintable)
struct FConversationTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName speaker = "Null";
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString line = "None";
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName animation = "None";
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UQuestAsset* questToGive;
};

UCLASS(BlueprintType, Blueprintable)
class FARMSIM_API UConversationAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UQuestAsset* questRequiredCompleted;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UDataTable* conversationTable;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltup = "Is this a filler conversation(step finished, step/quest needed, etc) or a real conversation"))
	bool bIsFiller = false;
};


USTRUCT(BlueprintType, Blueprintable)
struct FQuest
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UQuestAsset* Quest = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int curStep = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName questGiverName = "none";
};