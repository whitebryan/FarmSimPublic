// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QuestAndConvoStructs.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "ConversationManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FARMSIM_API UConversationManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UConversationManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	int lineInCurrentConversation = 0;

	UConversationAsset* curConversation;

	AActor* NPCTalking = nullptr;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void startConversation(UConversationAsset* conversationToStart, AActor* npcToTalkTo);

	UFUNCTION(BlueprintCallable)
	void endConversation();

	UFUNCTION(BlueprintCallable)
	FConversationTableRow getNextConversationLine();
};
