// Fill out your copyright notice in the Description page of Project Settings.


#include "ConversationManager.h"
#include "../Player/PlayerUIInterface.h"
#include "../AI/BaseAICharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h" 

// Sets default values for this component's properties
UConversationManager::UConversationManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UConversationManager::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UConversationManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UConversationManager::startConversation(UConversationAsset* conversationToStart, AActor* npcToTalkTo)
{
	if (conversationToStart == nullptr || conversationToStart->conversationTable == nullptr || npcToTalkTo == nullptr)
	{
		return;
	}

	curConversation = conversationToStart;
	NPCTalking = npcToTalkTo;
	lineInCurrentConversation = 0;
}

void UConversationManager::endConversation()
{
	if(curConversation == nullptr)
	{
		return;
	}

	curConversation = nullptr;
	lineInCurrentConversation = 0;
	AActor* player = Cast<AActor>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (IsValid(player) && player->GetClass()->ImplementsInterface(UPlayerUIInterface::StaticClass()))
	{
		IPlayerUIInterface::Execute_setPlayerUI(player, false, "Conversation", false);
	}
}

FConversationTableRow UConversationManager::getNextConversationLine()
{
	if(curConversation == nullptr)
	{
		return FConversationTableRow();
	}
	else if (lineInCurrentConversation >= curConversation->conversationTable->GetRowNames().Num())
	{
		endConversation();
		Cast<ABaseAICharacter>(NPCTalking)->conversationCompleted();
		return FConversationTableRow();
	}

	FName rowName = *FString::FromInt(lineInCurrentConversation);
	++lineInCurrentConversation;
	FConversationTableRow result = *curConversation->conversationTable->FindRow<FConversationTableRow>(rowName, " ");
	return result;
}
