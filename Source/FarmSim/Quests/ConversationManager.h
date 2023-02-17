// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "QuestComponent.h"
#include "ConversationManager.generated.h"

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
};

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

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void startConversation(UConversationAsset* conversationToStart);

	UFUNCTION(BlueprintCallable)
	void endConversation();

	UFUNCTION(BlueprintCallable)
	FConversationTableRow getNextConversationLine();
};
