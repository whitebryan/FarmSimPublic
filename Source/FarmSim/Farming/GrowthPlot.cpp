// Fill out your copyright notice in the Description page of Project Settings.


#include "GrowthPlot.h"
#include "../InventoryAndCrafting/InventoryItem.h"


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


bool AGrowthPlot::plantCrop(const FName cropID)
{
	FInvTableItem* curPlantRow = plantDataTable->FindRow<FInvTableItem>(cropID, FString(""));
	if (curPlantRow == nullptr)
	{
		return false;
	}

	USeedItemAsset* plantItem = Cast<USeedItemAsset>(curPlantRow->item);


	ABasePlant* newPlant;
	TArray<FString> availSeasons = plantItem->availSeasons;

	//Check if the plant can be planted in the current season then plant it if it can
	if (availSeasons.Contains(curSeason))
	{
		//Spawn given plant at base location
		newPlant =  GetWorld()->SpawnActor<ABasePlant>(curPlantRow->itemBlueprint, GetActorLocation(), GetActorRotation());
		newPlant->myPlot = this;
		newPlant->InitalizePlant(true);

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
		GetWorldTimerManager().SetTimer(timedRenable, this, &AGrowthPlot::timedRenableInteract, 0.5f, false);
	}
	else
	{
		myInteract->toggleInteractability();
		myCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AGrowthPlot::timedRenableInteract()
{
	myInteract->toggleInteractability();
	myCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

bool AGrowthPlot::isInUse()
{
	if (IsValid(myCollider) && myCollider->GetCollisionEnabled() == ECollisionEnabled::NoCollision)
	{
		return true;
	}
	else
	{
		return false;
	}
}