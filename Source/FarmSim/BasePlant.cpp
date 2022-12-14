// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlant.h"
#include "InteractInterface.h"
#include "GrowthPlot.h"
#include "TimerManager.h"
#include "InventoryItem.h"
#include "InventoryComponent.h"
#include "FarmSimCharacter.h"

// Sets default values
ABasePlant::ABasePlant()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ABasePlant::BeginPlay()
{
	Super::BeginPlay();

	//Only here for testing remove after
	InitalizePlant(FDateTime::Now(), true);
}

// Called every frame
void ABasePlant::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (curStatus == GrowthStatus::Watered && wateredTimer >= (secondsToGrow / timesToWater))
	{
		wateredTimer = 0;
		wateredPercent = 1;
		breakPointChange(curBreakPoint + 1);
	}
	else if(curStatus == GrowthStatus::Watered)
	{
		wateredTimer += DeltaTime;
		wateredPercent = (wateredTimer/(secondsToGrow/timesToWater));
	}
}

//Setups a plant and changes its growth status if being reloaded
void ABasePlant::InitalizePlant(FDateTime plantedTime, bool useCurTime)
{
	if (useCurTime)
	{
		plantedDate = FDateTime::Now();
		statusChange(GrowthStatus::NeedsWater);
	}
	else
	{
		toggleInteractibility();
		plantedDate = plantedTime;
		FDateTime curTime = FDateTime::Now();
		FTimespan curSpan = curTime - plantedDate;

		int totalTime = curSpan.GetSeconds() + (curSpan.GetHours() * 60 * 60) + (curSpan.GetMinutes() * 60) + (curSpan.GetDays() * 24 * 60 * 60);
		int breakPointLength = secondsToGrow / timesToWater;

		if (totalTime > secondsToGrow)
		{
			//Instantly finish growing
			breakPointChange(timesToWater);
			statusChange(GrowthStatus::Grown);
			return;
		}
		else if(totalTime > breakPointLength)
		{
			breakPointChange(totalTime / breakPointLength);
			statusChange(GrowthStatus::NeedsWater);
		}
	}
}


//Changes plant status 
void ABasePlant::breakPointChange(int newBreakPoint)
{
	curBreakPoint = newBreakPoint;
	if (curBreakPoint > timesToWater)
	{
		statusChange(GrowthStatus::Grown);
		toggleInteractibility();
	}
	else
	{
		statusChange(GrowthStatus::NeedsWater);
		toggleInteractibility();
	}
}


//Called from blueprint when interacted with
void ABasePlant::waterPlant()
{
	if (curStatus != GrowthStatus::NeedsWater)
		return;

	wateredPercent = 0;
	statusChange(GrowthStatus::Watered);
	toggleInteractibility();
}

//Called from blueprint when interacted with and done growing
void ABasePlant::harvestPlant()
{
	//Cast to player
	AFarmSimCharacter* player = Cast<AFarmSimCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	UInventoryComponent* playerInvComp;
	playerInvComp = Cast<UInventoryComponent>(player->GetComponentByClass(UInventoryComponent::StaticClass()));

	int harvestAmt = UKismetMathLibrary::RandomIntegerInRange(harvestMin, harvestMax);

	if (playerInvComp)
	{
		FInvTableItem* curPlantRow = plantDataTable->FindRow<FInvTableItem>(plantID, FString(""));
		FInvItem newItem;
		newItem.uniqueID = plantID;
		newItem.name = plantName;
		newItem.description = curPlantRow->description;
		newItem.quantity = harvestAmt;
		newItem.type = curPlantRow->type;
		newItem.icon = curPlantRow->icon;

		FAddItemStatus harvested = playerInvComp->addNewItem(newItem);
		FString harvestMessage;

		//if player inventory is full return else make the plot useable again
		if (!harvested.addStatus)
		{
			player->displayNotification("Inventory full");
			return;
		}
		else if (harvested.leftOvers > 0)
		{
			harvestMessage = "Harvested " + FString::FromInt(harvestAmt - harvested.leftOvers) + " " + plantName.ToString() + "(s)";
			harvestMessage += " and dropped " + FString::FromInt(harvested.leftOvers);
		}
		else
		{
			harvestMessage = "Harvested " + FString::FromInt(harvestAmt) + " " + plantName.ToString() + "(s)"; 
		}

		player->displayNotification(harvestMessage, 2);
		Cast<AGrowthPlot>(myPlot)->changeInteractability();
		Destroy();
	}
}

//Change the growth status and notifiy any potential sprinklers
void ABasePlant::statusChange(GrowthStatus newStatus)
{
	curStatus = newStatus;
	if(OnStatusUpdated.IsBound())
		OnStatusUpdated.Broadcast(curStatus);
	modelChange();
}

//Change the model to match status
void ABasePlant::modelChange()
{
	switch (curStatus)
	{
		case GrowthStatus::NeedsWater:
			plantMeshComp->SetStaticMesh(unwateredModel);
			break;
		case GrowthStatus::Watered:
			plantMeshComp->SetStaticMesh(wateredGrowingModel);
			break;
		case GrowthStatus::Grown:
			plantMeshComp->SetStaticMesh(grownModel);
			break;
		default:
			break;
	}
}
