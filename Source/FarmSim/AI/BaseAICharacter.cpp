// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAICharacter.h"
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
	if (curConversation >= 0 && curConversation < npcConversations.Num())
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
	if (IsValid(myController))
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
		}
	}
}
