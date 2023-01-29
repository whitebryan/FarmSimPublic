// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseSprinkler.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseSprinkler::ABaseSprinkler()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	SetActorTickEnabled(false);
}

// Called when the game starts or when spawned
void ABaseSprinkler::BeginPlay()
{
	Super::BeginPlay();
	playerChar = GetWorld()->GetFirstPlayerController()->GetPawn();
}

// Called every frame
void ABaseSprinkler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//if player is within 400 feet check for new plants to add to list
	float vectDist = UKismetMathLibrary::Vector_Distance(GetActorLocation(), playerChar->GetActorLocation());
	if (vectDist <= 300)
	{
		checkForPlants();
	}
}


//if delegate is called go through list and check if any plant needs water
void ABaseSprinkler::PlantStatusChanged(GrowthStatus newStatus)
{
	if (newStatus == GrowthStatus::NeedsWater)
	{
		for (ABasePlant* curPlant : plantsToWater)
		{
			GrowthStatus curStatus = curPlant->getCurStatus();
			if (curStatus == GrowthStatus::NeedsWater)
			{
				curPlant->waterPlant();
			}
			else if (curStatus == GrowthStatus::Grown)
			{
				plantsToWater.Remove(curPlant);
			}
		}
	}
}

void ABaseSprinkler::checkForPlants()
{
	for (TActorIterator<ABasePlant> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ABasePlant* myPlant = *ActorItr;
		float vectDist = UKismetMathLibrary::Vector_Distance(GetActorLocation(), myPlant->GetActorLocation());
		if (vectDist <= sprinklerRange)
		{
			plantsToWater.Add(myPlant);
			if(myPlant->getCurStatus() == GrowthStatus::NeedsWater)
				myPlant->waterPlant();

			myPlant->OnStatusUpdated.AddDynamic(this, &ABaseSprinkler::PlantStatusChanged);
		}
	}
}
