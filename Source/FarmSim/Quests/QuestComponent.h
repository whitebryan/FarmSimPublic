// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedStruct.h"
#include "GameplayTagContainer.h"
#include "../InventoryAndCrafting/InventoryItem.h"
#include "QuestComponent.generated.h"

/**
 * 
 */


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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip = "Tags required for quest completion"))
	FGameplayTagContainer requiredTags;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip = "Tag that restrict quest completion"))
	FGameplayTagContainer restrictedTags;
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
struct FQuest
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UQuestAsset* Quest;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int curStep = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStepCompleted, const FQuestStepStruct&, step);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestCompleted, UQuestAsset*, quest);
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FARMSIM_API UQuestTrackerComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UQuestTrackerComponent();

	UFUNCTION(BlueprintCallable)
	void startQuest(const FQuest& newQuest);
	UFUNCTION(BlueprintCallable, meta = (Tooltip = "Returns a QuestStepStruct with only the unfullfilled requirements"))
	FQuestStepStruct checkRemainingProgress(const FString questName);
	UFUNCTION(BlueprintCallable)
	void turnInQuestStep(const FString questName);
	
	UPROPERTY(BlueprintAssignable)
	FOnQuestCompleted questCompleted;
	UPROPERTY(BlueprintAssignable)
	FOnStepCompleted stepCompleted;

protected:

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere)
	TMap<FString, FQuest> quests;
};