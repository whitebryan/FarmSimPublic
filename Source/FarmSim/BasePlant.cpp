// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlant.h"
#include "InteractInterface.h"
#include "GrowthPlot.h"
#include "TimerManager.h"
#include "InventoryComponent.h"
#include "FarmSimCharacter.h"

// Sets default values
ABasePlant::ABasePlant()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);
}

// Called when the game starts or when spawned
void ABasePlant::BeginPlay()
{
	Super::BeginPlay();
	timePerBreakPoint = ((float)plantToGrow->secondsToGrow / (1 + plantToGrow->timesToWater));
}

// Called every frame
void ABasePlant::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (curStatus == GrowthStatus::Watered && wateredTimer >= timePerBreakPoint)
	{
		wateredTimer = 0;
		wateredPercent = 1;
		breakPointChange(curBreakPoint + 1);
	}
	else if(curStatus == GrowthStatus::Watered)
	{
		totalSecondsGrowing += DeltaTime;
		wateredTimer += DeltaTime;
		wateredPercent = wateredTimer/timePerBreakPoint;
	}
}

//Setups a plant and changes its growth status if being reloaded
void ABasePlant::InitalizePlant(bool newPlant, AActor* newPlot, float Timer)
{
	if (newPlant)
	{
		statusChange(GrowthStatus::NeedsWater);
	}
	else
	{
		myPlot = newPlot;
		Cast<AGrowthPlot>(myPlot)->changeInteractability();

		float BreakPoint = Timer / timePerBreakPoint;
		totalSecondsGrowing = Timer;

		toggleInteractibility();

		if (fmod(Timer, timePerBreakPoint) > 0)
		{
			wateredTimer = fmod(Timer, timePerBreakPoint);
			wateredPercent = wateredTimer/timePerBreakPoint;
			statusChange(GrowthStatus::Watered);
			curBreakPoint = BreakPoint;
		}
		else
		{
			breakPointChange(BreakPoint);
		}
	}
}


//Changes plant status 
void ABasePlant::breakPointChange(int newBreakPoint)
{
	curBreakPoint = newBreakPoint;
	if (curBreakPoint > plantToGrow->timesToWater)
	{
		statusChange(GrowthStatus::Grown);
		Cast<UBoxComponent>(GetComponentByClass(UBoxComponent::StaticClass()))->ComponentTags.Add("Interaction");
		toggleInteractibility();
	}
	else
	{
		statusChange(GrowthStatus::NeedsWater);
		toggleInteractibility();
	}

	totalSecondsGrowing = timePerBreakPoint * curBreakPoint;
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

	int harvestAmt = UKismetMathLibrary::RandomIntegerInRange(plantToGrow->harvestMin, plantToGrow->harvestMax);

	if (playerInvComp)
	{
		//FInvTableItem* curPlantRow = plantDataTable->FindRow<FInvTableItem>(plantToGrow->plantID, FString(""));
		FInvItem newItem;
		newItem.item = plantToGrow;
		newItem.quantity = harvestAmt;

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
			harvestMessage = "Harvested " + FString::FromInt(harvestAmt - harvested.leftOvers) + " " + plantToGrow->name.ToString() + "(s)";
			harvestMessage += " and dropped " + FString::FromInt(harvested.leftOvers);
		}
		else
		{
			harvestMessage = "Harvested " + FString::FromInt(harvestAmt) + " " + plantToGrow->name.ToString() + "(s)";
		}

		Cast<UPlayerSaveManagerComponent>(player->GetComponentByClass(UPlayerSaveManagerComponent::StaticClass()))->tryDiscoverItem(plantToGrow);
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
			SetActorTickEnabled(true);
			plantMeshComp->SetStaticMesh(plantToGrow->unwateredModel);
			break;
		case GrowthStatus::Watered:
			SetActorTickEnabled(true);
			plantMeshComp->SetStaticMesh(plantToGrow->wateredGrowingModel);
			break;
		case GrowthStatus::Grown:
			SetActorTickEnabled(false);
			plantMeshComp->SetStaticMesh(plantToGrow->grownModel);
			break;
		default:
			break;
	}
}
