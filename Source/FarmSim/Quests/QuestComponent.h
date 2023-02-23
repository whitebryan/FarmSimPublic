// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedStruct.h"
#include "GameplayTagContainer.h"
#include "QuestAndConvoStructs.h"
#include "../InventoryAndCrafting/InventoryItem.h"
#include "QuestComponent.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStepCompleted, const FQuestStepStruct&, step, const FString&, questName);
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
	bool turnInQuestStep(const FString& questName);

	UFUNCTION(BlueprintCallable)
	int getCurStep(const FString& questName);
	
	UPROPERTY(BlueprintAssignable)
	FOnQuestCompleted questCompleted;
	UPROPERTY(BlueprintAssignable)
	FOnStepCompleted stepCompleted;

	UFUNCTION(BlueprintCallable)
	bool checkForCompletedQuest(const FString& questName);

	UFUNCTION(BlueprintCallable)
	const FQuest getQuestStatus(const FString& questName);

	void loadQuests(const TMap<FString, FQuest> loadedQuests) { quests = loadedQuests; }

protected:

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FString, FQuest> quests;
};