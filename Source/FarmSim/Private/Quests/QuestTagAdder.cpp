// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestTagAdder.h"
#include "Components/BoxComponent.h" 
#include "Components/CapsuleComponent.h" 
#include "Kismet/GameplayStatics.h"
#include "../Player/FarmSimCharacter.h"

// Sets default values
AQuestTagAdder::AQuestTagAdder()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AQuestTagAdder::BeginPlay()
{
	Super::BeginPlay();

	if (tagToAdd == FGameplayTag::EmptyTag)
	{
		UKismetSystemLibrary::PrintWarning("Auto destroying tag adder because it either had no tag");
		Destroy();
	}
	
	UBoxComponent* myBoxComponent = Cast<UBoxComponent>(GetComponentByClass(UBoxComponent::StaticClass()));
	myBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AQuestTagAdder::OnOverlapBegin);
}

// Called every frame
void AQuestTagAdder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AQuestTagAdder::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFarmSimCharacter* playerChar = Cast<AFarmSimCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (OtherActor == playerChar && OtherComp->GetClass() == UCapsuleComponent::StaticClass())
	{
		if (requiredInProgressQuest != nullptr)
		{
			TArray<AActor*> foundActors;
			UGameplayStatics::GetAllActorsWithTag(GetWorld(), "WorldManager", foundActors);

			if (foundActors.Num() > 0)
			{
				UQuestTrackerComponent* questTracker = Cast<UQuestTrackerComponent>(foundActors[0]->GetComponentByClass(UQuestTrackerComponent::StaticClass()));
				if (IsValid(questTracker))
				{
					FQuest questStatus = questTracker->getQuestStatus(requiredInProgressQuest->questName);

					if (questStatus.Quest != nullptr && questStatus.curStep < questStatus.Quest->questSteps.Num())
					{
						playerChar->playerTags.AddTag(tagToAdd);
						
						if (bDestroyAfterUse)
						{
							Destroy();
						}
					}
					else
					{
						return;
					}
				}
			}
		}
		else
		{
			playerChar->playerTags.AddTag(tagToAdd);

			if (bDestroyAfterUse)
			{
				Destroy();
			}
		}
	}
}
