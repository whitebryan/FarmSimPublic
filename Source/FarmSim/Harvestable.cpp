// Fill out your copyright notice in the Description page of Project Settings.


#include "Harvestable.h"
#include "FarmSimCharacter.h"
#include "InventoryComponent.h"

// Sets default values
AHarvestable::AHarvestable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AHarvestable::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AHarvestable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHarvestable::tryHarvest() //Check if the players tool is able to harvest then harvest add to their inventory and respawn if set to respawn
{
	AFarmSimCharacter* player = Cast<AFarmSimCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (player->getEquippeddTool() == requiredToolStatus || !requiresTool)
	{
		FToolInvItem playerTool = player->grabTool(toolType);
		if (playerTool.type == toolType || !requiresTool)
		{
			if (playerTool.toolTier >= toolLevel || !requiresTool)
			{
				UInventoryComponent* playerInvComp = Cast<UInventoryComponent>(player->GetComponentByClass(UInventoryComponent::StaticClass()));

				if (IsValid(playerInvComp))
				{
					if (playerInvComp->getItemQuantity(itemName) == 0)
					{
						FInvTableItem* curItemRow = harvestablesTable->FindRow<FInvTableItem>(itemName, FString(""));
						if (curItemRow == nullptr)
						{
							return;
						}

						FInvItem newItem;
						newItem.name = itemName;
						newItem.description = curItemRow->description;

						if (!requiresTool)
						{
							newItem.quantity = harvestAmount;
						}
						else
						{
							newItem.quantity = harvestAmount + ((uint8)playerTool.toolTier - (uint8)toolLevel) * 2;
						}

						newItem.type = curItemRow->type;
						newItem.icon = curItemRow->icon;

						bool harvested = playerInvComp->addNewItem(newItem);

						if (!harvested)
						{
							player->displayNotification("Inventory full", 2);
							return;
						}
						else
						{
							FString returnMessage = "Harvested " + FString::FromInt(newItem.quantity) + " " + itemName.ToString() + "(s)";
							changeStatus(false);
							if (!respawnAble)
							{
								Destroy();
								player->displayNotification(returnMessage, 2);
								return;
							}
							else
							{
								GetWorldTimerManager().SetTimer(respawnTimer, this, &AHarvestable::respawn, respawnTime, false);
								player->displayNotification(returnMessage, 2);
								return;
							}
						}
					}
					else
					{
						int amt;

						if (!requiresTool)
						{
							amt = harvestAmount;
						}
						else
						{
							amt = harvestAmount + ((uint8)playerTool.toolTier - (uint8)toolLevel) * 2;
						}
						int leftovers = playerInvComp->changeQuantity(itemName, amt);

						FString returnMessage = "Harvested " + FString::FromInt(amt - leftovers) + " " + itemName.ToString() + "(s)";

						if (leftovers > 0)
						{
							FInvTableItem* curItemRow = harvestablesTable->FindRow<FInvTableItem>(itemName, FString(""));
							FInvItem newItem;
							newItem.name = itemName;
							newItem.description = curItemRow->description;
							newItem.quantity = harvestAmount + ((uint8)playerTool.toolTier - (uint8)toolLevel) * 2;
							newItem.type = curItemRow->type;
							newItem.icon = curItemRow->icon;

							playerInvComp->createLootBag(newItem);

							returnMessage +='\n' + "Dropped " + FString::FromInt(leftovers);
						}

						changeStatus(false);
						if (!respawnAble)
						{
							Destroy();
							player->displayNotification(returnMessage, 2);
							return;
						}
						else
						{
							GetWorldTimerManager().SetTimer(respawnTimer, this, &AHarvestable::respawn, respawnTime, false);
							player->displayNotification(returnMessage, 2);
							return;
						}
					}
				}
			}
			else
			{
				FString returnMessage = "You require a ";
				returnMessage += toolTierToString() + " tier tool to harvest this";
				player->displayNotification(returnMessage, 2);
				return;
			}
		}
		else
		{
			FString returnMessage = "This requires a " + toolTypeToString() + " to harvest";
			player->displayNotification(returnMessage, 2);
		}
	}
	else
	{
		FString returnMessage = "This requires a " + toolTypeToString() + " to harvest";
		player->displayNotification(returnMessage, 2);
	}
}

//Make interacble again if respawnable
void AHarvestable::respawn()
{
	changeStatus(true);
}

//Change model and collision mode based off status
void AHarvestable::changeStatus(bool harvestable)
{
	if (harvestable)
	{
		boxCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		meshComponent->SetStaticMesh(harvestableModel);
	}
	else
	{
		boxCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		meshComponent->SetStaticMesh(harvestedModel);
	}
}


//Simply converting toolType or toolTier to a string for use in return messages because I found no way to grab the display name of a TEnumAsByte
FString AHarvestable::toolTypeToString()
{
	switch (requiredToolStatus)
	{
	case PlayerToolStatus::AxeOut:
		return "Axe";
	case PlayerToolStatus::PickaxeOut:
		return "Pickaxe";
	case PlayerToolStatus::ShovelOut:
		return "Shovel";
	default:
		return "Error";
		break;
	}
}

FString AHarvestable::toolTierToString()
{
	switch (toolLevel)
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