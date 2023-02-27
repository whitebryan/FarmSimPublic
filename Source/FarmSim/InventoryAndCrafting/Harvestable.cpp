// Fill out your copyright notice in the Description page of Project Settings.


#include "Harvestable.h"
#include "../Player/FarmSimCharacter.h"
#include "InventoryComponent.h"

// Sets default values
AHarvestable::AHarvestable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);
}

// Called when the game starts or when spawned
void AHarvestable::BeginPlay()
{
	Super::BeginPlay();

	myInteractComp = Cast<UInteractComponent>(GetComponentByClass(UInteractComponent::StaticClass()));

	if (IsValid(itemToHarvest))
	{
		meshComponent->SetStaticMesh(itemToHarvest->harvestableModel);
		toolType = FName(*toolTypeToString());
	}
	else
	{
		Destroy();
	}
}

void AHarvestable::initHarvestable(float newTime)
{
	changeStatus(false);
	respawnTimer = newTime;
	SetActorTickEnabled(true);
}


// Called every frame
void AHarvestable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (respawnTimer >= itemToHarvest->respawnTime)
	{
		respawnTimer = 0;
		changeStatus(true);
		SetActorTickEnabled(false);
	}
	else
	{
		respawnTimer += DeltaTime;
	}
}

void AHarvestable::tryHarvest() //Check if the players tool is able to harvest then harvest add to their inventory and respawn if set to respawn
{
	AFarmSimCharacter* player = Cast<AFarmSimCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!itemToHarvest->requiresTool || player->findTagOfType(player->toolStatusTag).MatchesTagExact(itemToHarvest->requiredToolStatus))
	{
		UToolItemAsset* playerTool = player->grabTool(itemToHarvest->requiredToolStatus);
		if (!itemToHarvest->requiresTool || playerTool->type == toolType)
		{
			if (!itemToHarvest->requiresTool || playerTool->toolTier >= itemToHarvest->toolLevel)
			{
				if (bShouldMoveInteractee && myInteractComp->GetClass()->ImplementsInterface(UInteractInterface::StaticClass()))
				{
					myInteractComp->Execute_moveActorIntoPlace(myInteractComp, Cast<AActor>(player));
				}

				if (itemToHarvest->requiresTool)
				{
					Cast<AFarmSimCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))->toolUsed = true;
				}

				UInventoryComponent* playerInvComp = Cast<UInventoryComponent>(player->GetComponentByClass(UInventoryComponent::StaticClass()));

				if (IsValid(playerInvComp))
				{
					FInvItem newItem;
					newItem.item = itemToHarvest;

					if (!itemToHarvest->requiresTool)
					{
						newItem.quantity = itemToHarvest->harvestAmount;
					}
					else
					{
						newItem.quantity = itemToHarvest->harvestAmount + ((uint8)playerTool->toolTier - (uint8)itemToHarvest->toolLevel) * 2;
					}

					FAddItemStatus harvested = playerInvComp->addNewItem(newItem);

					if (!harvested.addStatus)
					{
						player->displayNotification("Inventory full", 1);
						return;
					}
					else
					{
						Cast<UPlayerSaveManagerComponent>(player->GetComponentByClass(UPlayerSaveManagerComponent::StaticClass()))->tryDiscoverItem(itemToHarvest);

						FString returnMessage = "Harvested " + FString::FromInt(newItem.quantity) + " " + itemToHarvest->name.ToString() + "(s)";
						if (harvested.leftOvers > 0)
						{
							returnMessage += '\n' + "Dropped " + FString::FromInt(harvested.leftOvers);
						}

						changeStatus(false);
						if (!itemToHarvest->respawnAble)
						{
							Destroy();
							player->displayNotification(returnMessage, 1);
							return;
						}
						else
						{
							player->displayNotification(returnMessage, 1);
							return;
						}
					}
				}
			}
			else
			{
				FString returnMessage = "You require a ";
				returnMessage += toolTierToString() + " tier tool to harvest this";
				player->displayNotification(returnMessage, 1);
				return;
			}
		}
		else
		{
			FString returnMessage = "This requires a " + toolTypeToString() + " to harvest";
			player->displayNotification(returnMessage, 1);
		}
	}
	else
	{
		FString returnMessage = "This requires a " + toolTypeToString() + " to harvest";
		player->displayNotification(returnMessage, 1);
	}
}

//Change model and collision mode based off status
void AHarvestable::changeStatus(bool harvestable)
{
	if (harvestable)
	{
		boxCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		meshComponent->SetStaticMesh(itemToHarvest->harvestableModel);
	}
	else
	{
		boxCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetWorldTimerManager().SetTimer(meshChangeTimer, this, &AHarvestable::delayedMeshChange, harvestedMeshChangeDelay);
	}
}

void AHarvestable::delayedMeshChange()
{
	SetActorTickEnabled(true);
	meshComponent->SetStaticMesh(itemToHarvest->harvestedModel);
}

//Simply converting toolType or toolTier to a string for use in return messages because I found no way to grab the display name of a TEnumAsByte
FString AHarvestable::toolTypeToString()
{
	FString toolKey;
	FString toolTypeString;

	itemToHarvest->requiredToolStatus.GetTagName().ToString().Split(FString("."), &toolKey, &toolTypeString);

	if (toolTypeString == "Axe")
	{
		return "Axe";
	}
	else if (toolTypeString == "Pickaxe")
	{
		return "Pickaxe";
	}
	else if (toolTypeString == "Pickaxe")
	{
		return "Shovel";
	}
	else
	{
		return "Error";
	}
}

FString AHarvestable::toolTierToString()
{
	switch (itemToHarvest->toolLevel)
	{
	case ToolTier::Starter:
		return "Starter";
	case ToolTier::Bronze:
		return "Bronze";
	case ToolTier::Silver:
		return "Silver";
	case ToolTier::Gold:
		return "Gold";
	case ToolTier::Diamond:
		return "Diamond";
	default:
		return "Error";
		break;
	}
}

float AHarvestable::getCurTimeTillRespawn()
{
	return respawnTimer;
}