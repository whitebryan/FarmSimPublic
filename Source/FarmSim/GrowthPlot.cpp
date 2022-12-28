// Fill out your copyright notice in the Description page of Project Settings.


#include "GrowthPlot.h"
#include "InventoryItem.h"

// Sets default values
AGrowthPlot::AGrowthPlot()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AGrowthPlot::BeginPlay()
{
	Super::BeginPlay();
	myInteract = FindComponentByClass<UInteractComponent>();
	myCollider = FindComponentByClass<UBoxComponent>();
}

// Called every frame
void AGrowthPlot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


bool AGrowthPlot::plantCrop(FName cropName)
{
	FInvTableItem* curPlantRow = plantDataTable->FindRow<FInvTableItem>(cropName, FString(""));

	//Return if data table is faulty
	if (curPlantRow == nullptr)
	{
		return false;
	}

	ABasePlant* newPlant;
	TArray<FString> availSeasons = curPlantRow->availSeasons;

	//Check if the plant can be planted in the current season then plant it if it can
	if (availSeasons.Contains(curSeason))
	{
		//Spawn given plant at base location
		newPlant =  GetWorld()->SpawnActor<ABasePlant>(curPlantRow->itemBlueprint, GetActorLocation(), GetActorRotation());
		newPlant->myPlot = this;

		//Toggle interactability while a crop is growing here
		if (myInteract)
		{
			changeInteractability();
		}

		return true;
	}

	return false;
}

void AGrowthPlot::changeInteractability()
{
	if (myCollider->GetCollisionEnabled() == ECollisionEnabled::NoCollision)
	{
		myInteract->toggleInteractability();
		myCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	else
	{
		myInteract->toggleInteractability();
		myCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

bool AGrowthPlot::isInUse()
{
	if (myCollider->GetCollisionEnabled() == ECollisionEnabled::NoCollision)
	{
		return true;
	}
	else
	{
		return false;
	}
}