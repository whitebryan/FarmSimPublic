// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAICharacter.h"
#include "FarmSimCharacter.h"
#include "QuestComponent.h"
#include "BaseNPCController.h"

// Sets default values
ABaseAICharacter::ABaseAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseAICharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABaseAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABaseAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UConversationAsset* ABaseAICharacter::getCurConversation()
{
	if (curConversation < npcConversations.Num() && npcConversations[curConversation]->questRequiredCompleted != nullptr)
	{
		TArray<AActor*> foundActors;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), "WorldManager", foundActors);
		if (foundActors.Num() > 0)
		{
			UQuestTrackerComponent* questTracker = Cast<UQuestTrackerComponent>(foundActors[0]->GetComponentByClass(UQuestTrackerComponent::StaticClass()));

			bool turnInStep = questTracker->turnInQuestStep(npcConversations[curConversation]->questRequiredCompleted->questName);

			if (questTracker->checkForCompletedQuest(npcConversations[curConversation]->questRequiredCompleted->questName))
			{
				return npcConversations[curConversation];
			}
			else if(turnInStep)
			{	
				int curStep = questTracker->getCurStep(npcConversations[curConversation]->questRequiredCompleted->questName);

				return npcConversations[curConversation]->questRequiredCompleted->questSteps[curStep-1].stepCompleted;
			}
			else
			{
				--curConversation;
				return stepNeededConversation;
			}
		}
		else
		{
			return nullptr;
		}
	}
	else if (curConversation >= 0 && curConversation < npcConversations.Num())
	{
		return npcConversations[curConversation];
	}
	else
	{
		return nullptr;
	}
}

void ABaseAICharacter::Interact_Implementation()
{
	ABaseNPCController* myController = Cast<ABaseNPCController>(GetController());
	if (IsValid(myController) && curConversation < npcConversations.Num())
	{
		bool bIsTalking = myController->myBlackboard->GetValueAsBool("bIsTalking");
		if (bIsTalking)
		{
			myController->myBlackboard->SetValueAsBool("bIsTalking", false);
			myController->objectToTrack = nullptr;
			myController->bShouldCheckForHeadTracking = false;
		}
		else
		{
			myController->myBlackboard->SetValueAsBool("bIsTalking", true);
			myController->objectToTrack = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
			myController->bShouldCheckForHeadTracking = true;
			rotateToFace(myController->objectToTrack);
			Cast<AFarmSimCharacter>(myController->objectToTrack)->conversationControl("Start");
		}
	}
}

void ABaseAICharacter::conversationCompleted()
{
	Interact();
	if (bShouldLoopAllConversations)
	{
		++curConversation;
		if (curConversation >= npcConversations.Num())
		{
			curConversation = 0;
		}
	}
	else if(bShouldLoopLastConversation)
	{
		curConversation = FMath::Clamp(++curConversation, 0, npcConversations.Num()-1);
	}
	else
	{
		++curConversation;
	}

	AFarmSimCharacter* player = Cast<AFarmSimCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	UPlayerSaveManagerComponent* playerSaveManager = Cast<UPlayerSaveManagerComponent>(player->GetComponentByClass(UPlayerSaveManagerComponent::StaticClass()));
	playerSaveManager->saveNPCConversationStatus(npcName, curConversation);
}
