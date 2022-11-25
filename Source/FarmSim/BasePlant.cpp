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

	statusChange(GrowthStatus::Watered);
	toggleInteractibility();

	if (GetWorld())
	{
		FTimerDelegate TimerDel;      
		TimerDel.BindUFunction(this, "breakPointChange", curBreakPoint + 1);

		int timerLen = secondsToGrow / (timesToWater + 1);

		GetWorldTimerManager().SetTimer(wateredTimer, TimerDel, timerLen, false);
	}
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
		if (playerInvComp->getItemQuantity(plantName) == 0)
		{
			FInvTableItem* curPlantRow = plantDataTable->FindRow<FInvTableItem>(plantName, FString(""));
			FInvItem newItem;
			newItem.name = plantName;
			newItem.quantity = harvestAmt;
			newItem.type = curPlantRow->type;
			newItem.icon = curPlantRow->icon;

			bool harvested = playerInvComp->addNewItem(newItem);

			//if player inventory is full return else make the plot useable again
			if (!harvested)
			{
				player->displayNotification("Inventory full");
				return;
			}
			else
			{
				FString harvestMessage = "Harvested " + FString::FromInt(harvestAmt) + " " + plantName.ToString() + "(s)"; 
				player->displayNotification(harvestMessage, 2);
				Cast<AGrowthPlot>(myPlot)->changeInteractability();
				Destroy();
			}
		}
		else 
		{
			int leftovers = playerInvComp->changeQuantity(plantName, harvestAmt);
			Cast<AGrowthPlot>(myPlot)->changeInteractability();
			
			FString harvestMessage = "Harvested " + FString::FromInt(harvestAmt - leftovers) + " " + plantName.ToString() + "(s)";

			if (leftovers > 0)
			{
				FInvTableItem* curPlantRow = plantDataTable->FindRow<FInvTableItem>(plantName, FString(""));
				FInvItem newItem;
				newItem.name = plantName;
				newItem.quantity = leftovers;
				newItem.type = curPlantRow->type;
				newItem.icon = curPlantRow->icon;

				playerInvComp->createLootBag(newItem);
				harvestMessage += " and dropped " + FString::FromInt(leftovers);
			}

			player->displayNotification(harvestMessage, 2);

			Destroy();
		}
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
