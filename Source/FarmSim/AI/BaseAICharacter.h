// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine/DataTable.h"
#include "../Quests/ConversationManager.h"
#include "../../Plugins/SimpleInteract/Source/SimpleInteract/Public/InteractInterface.h"
#include "BaseAICharacter.generated.h"


USTRUCT(BlueprintType, Blueprintable)

struct FScheduledLocationStruct
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector location = FVector(0,0,0);
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float wanderRange = 300;
};

USTRUCT(BlueprintType, Blueprintable)
struct FAIScheduleRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<int, FScheduledLocationStruct> normalLocations;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<int, FScheduledLocationStruct> stormingLocations;
};

UCLASS()
class FARMSIM_API ABaseAICharacter : public ACharacter, public IInteractInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseAICharacter();


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Interact(); virtual void Interact_Implementation() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	int curConversation = 0;

	UPROPERTY(EditAnywhere, Category = "Conversation")
	UConversationAsset* questNeededConversation;

	UPROPERTY(EditAnywhere, Category = "Conversation")
	UConversationAsset* stepNeededConversation;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, Category = "Conversation")
	FString npcName = "BaseNPC";

	UPROPERTY(BlueprintReadOnly)
	bool bShouldHeadTrack = false;

	UPROPERTY(BlueprintReadWrite)
	FName curInteractionType = "None";

	UPROPERTY(BlueprintReadWrite)
	bool bIsWalking = false;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* weeklySchedule;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> interactionTypes;

	void loadCurConvo(int cur) { curConversation = cur; }

	//Functions to talk to BP day/night system and weather system
	UFUNCTION(BlueprintImplementableEvent)
	int getDayOfTheWeek() const;

	UFUNCTION(BlueprintImplementableEvent)
	int getHourOfDay() const;

	UFUNCTION(BlueprintImplementableEvent)
	FName getSeason() const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	bool IsStorming();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conversation")
	TArray<UConversationAsset*> npcConversations;
	UPROPERTY(EditAnywhere, Category = "Conversation", meta = (Tooltip = "Do not loop if the conversation gives a quest you dont want to be repeatable"))
	bool bShouldLoopAllConversations = false;
	UPROPERTY(EditAnywhere, Category = "Conversation")
	bool bShouldLoopLastConversation = true;
	UFUNCTION(BlueprintCallable)
	void nextConversation() { ++curConversation; }
	UFUNCTION(BlueprintCallable)
	UConversationAsset* getCurConversation();
	UFUNCTION(BlueprintCallable)
	void conversationCompleted();

	UFUNCTION(BlueprintImplementableEvent)
	void rotateToFace(AActor* objectToFace);
};
