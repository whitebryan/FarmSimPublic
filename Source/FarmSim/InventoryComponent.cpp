// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "Kismet/KismetMathLibrary.h" 

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	emptyItem = FInvItem();

	//Init array with empty place holder items
	for (int i = 0; i < (inventoryRows * 5); ++i)
	{
		inventoryArray.Add(emptyItem);
		OnInvChanged.Broadcast();
	}
}

//Returns true if open slot and added, false otherwise
bool UInventoryComponent::addNewItem(FInvItem newItem)
{
	//Put it in the first empty position
	for(int i = 0; i < inventoryArray.Num(); ++i)
	{
		if (inventoryArray[i].name == FName("Empty"))
		{
			inventoryArray[i] = newItem;
			OnInvChanged.Broadcast();
			return true;
		}
	}

	return false;
}



void UInventoryComponent::moveItem(int from, int to)
{
	FInvItem prevItem = inventoryArray[to];

	//If items are the same item combine stacks if possible
	if (prevItem.name == inventoryArray[from].name)
	{
		//If combined they are a full stack or less combine into one stack, otherwise move a quantity
		if (prevItem.quantity + inventoryArray[from].quantity <= 99)
		{
			inventoryArray[to].quantity += inventoryArray[from].quantity;
			removeItem(from);
		}
		else if( prevItem.quantity != 99 || inventoryArray[from].quantity != 99)
		{
			int amountToLeave = (prevItem.quantity + inventoryArray[from].quantity ) - 99;
			inventoryArray[to].quantity = 99;
			inventoryArray[from].quantity = amountToLeave;
		}
	}
	else
	{
		inventoryArray[to] = inventoryArray[from];
		inventoryArray[from] = prevItem;
	}

	OnInvChanged.Broadcast();
}

void UInventoryComponent::removeItem(int slot)
{
	inventoryArray[slot] = emptyItem;
	OnInvChanged.Broadcast();
}

int UInventoryComponent::getItemQuantity(FName itemType)
{
	int curAmt = 0;
	for(int i = 0; i < inventoryArray.Num(); ++i)
	{
		if (inventoryArray[i].name == itemType)
		{
			curAmt += inventoryArray[i].quantity;
		}
	}
	return curAmt;
}

FInvItem UInventoryComponent::getItemAtSlot(int slot)
{
	if(slot > inventoryArray.Num() || slot < 0)
		return FInvItem();
	else
		return inventoryArray[slot];
}


//Crashes if called from C++ for some reason
//-1 returned means no item of type found
int UInventoryComponent::findNextItemOfType(int startPos, int direction, FName type)
{
	//-2 is a arbitrary number just used to get the first item of found of this type
	if((startPos < 0 && startPos != -2) || startPos >= inventoryArray.Num())
		return -1;

	bool looped = false;

	for (int i = startPos; i <= inventoryArray.Num(); i = i + (1 * direction))
	{
		if (i == -2)
		{
			i = -1;
			continue;
		}
		if (i == -1 && !looped)
		{
			i = inventoryArray.Num();
			looped = true;
			continue;
		}
		else if (i == inventoryArray.Num() && !looped)
		{
			i = -1;
			looped = true;
			continue;
		}
		else if (i == startPos && looped)
		{
			return -1;
		}
		else if (startPos == -2 && i == inventoryArray.Num() && looped)
		{
			return -1;
		}

		if (inventoryArray[i].type == type && i != startPos)
		{
			return i;
		}
	
	}

	return -1;
}

//Cannot add or remove MORE than 99 at one time
//When removing assume this is ONLY called if there is enough to remove
//When adding there can be leftovers to create new stack
//Returns leftovers in the case of a full inventory 
//Return -1 means there is no item of this type in inventory
//Return -2 means you tried to change more than 99 at one time
int UInventoryComponent::changeQuantity(FName itemType, int quantityToChange)
{
	if(quantityToChange > 99)
		return -2;
	
	//Checks for existing stacks to edit first
	int amountLeftToChange = quantityToChange;

	for(int i = 0; i < inventoryArray.Num(); ++i)
	{
		if (inventoryArray[i].name == itemType)
		{
			int curAmt = inventoryArray[i].quantity;

			if (curAmt + amountLeftToChange == 0)
			{
				removeItem(i);
				OnInvChanged.Broadcast();
				return 0;
			}
			else if (curAmt + amountLeftToChange < 0)
			{
				amountLeftToChange = amountLeftToChange - inventoryArray[i].quantity;
				removeItem(i);
				OnInvChanged.Broadcast();
				i = 0;
			}
			else if(curAmt + amountLeftToChange > 99 && curAmt != 99)
			{
				amountLeftToChange = 99 - inventoryArray[i].quantity;
				inventoryArray[i].quantity = 99;
				OnInvChanged.Broadcast();
			}
			else if (UKismetMathLibrary::SignOfInteger(amountLeftToChange) < 0)
			{
				inventoryArray[i].quantity += amountLeftToChange;
				OnInvChanged.Broadcast();
				return 0;
			}
			else if(inventoryArray[i].quantity != 99)
			{
				inventoryArray[i].quantity += amountLeftToChange;
				OnInvChanged.Broadcast();
				return 0;
			}
		}
	}

	//Leftovers remain after adding to existing stacks
	if (amountLeftToChange > 0)
	{
		FInvItem tempCopy = FInvItem();
		int emptySlot = -1;

		for(int i = 0; i < inventoryArray.Num(); ++i)
		{
			if (inventoryArray[i].name == FName("Empty") && emptySlot == -1)
			{
				emptySlot = i;
			}
			else if (inventoryArray[i].name == itemType)
			{
				tempCopy = inventoryArray[i];
			}
		}

		if (emptySlot == -1)
		{
			return amountLeftToChange;
		}
		else if (tempCopy.name == FName("Empty"))
		{
			return -1;
		}
		else
		{
			inventoryArray[emptySlot] = tempCopy;
			inventoryArray[emptySlot].quantity = amountLeftToChange;
			OnInvChanged.Broadcast();
			return 0;
		}
	}
	//should never reach this
	return -3;
}

//Split position into two stacks
bool UInventoryComponent::splitStack(int slot, int newStackSize)
{
	if(slot < 0 || slot >= inventoryArray.Num() || newStackSize >= inventoryArray[slot].quantity)
		return false;

	//Find empty spot then split or display error if no slots
	for (int i = 0; i < inventoryArray.Num(); ++i)
	{
		if (inventoryArray[i].name == "Empty")
		{
			inventoryArray[i] = inventoryArray[slot];
			inventoryArray[i].quantity = newStackSize;
			inventoryArray[slot].quantity = inventoryArray[slot].quantity - newStackSize;
			OnInvChanged.Broadcast();
			return true;
		}
	}

	//Display inventory full error
	return false;
}


//Move from one inventory to another for usage with chests
bool UInventoryComponent::moveToNewInvComp(int slot, UInventoryComponent* newComp)
{
	//Check if any stacks already exist and add to them if possible
	if (newComp->getItemQuantity(inventoryArray[slot].name) > 0)
	{
		int leftovers = newComp->changeQuantity(inventoryArray[slot].name, inventoryArray[slot].quantity);

		if (leftovers != 0)
		{
			inventoryArray[slot].quantity = leftovers;
		}
		else
		{
			removeItem(slot);
		}

		OnInvChanged.Broadcast();
		return true;
	}
	else
	{
		bool moving = newComp->addNewItem(inventoryArray[slot]);

		if (moving)
		{
			removeItem(slot);
			OnInvChanged.Broadcast();
			return true;
		}
		else
		{
			OnInvChanged.Broadcast();
			return false;
		}
	}
}


//Function to allow the user to drop items on the ground or for say plants to request a loot bag dropped if the new amount would overflow
void UInventoryComponent::createLootBag(FInvItem itemToDrop, int slot)
{
	FVector spawnLoc = GetOwner()->GetActorLocation() + ( GetOwner()->GetActorForwardVector() * 200 );
	AActor* newLootBag = GetWorld()->SpawnActor<AActor>(lootBag, spawnLoc, GetOwner()->GetActorRotation());
	
	UActorComponent* newInvComp = newLootBag->GetComponentByClass(UInventoryComponent::StaticClass());
	bool tryDrop = Cast<UInventoryComponent>(newInvComp)->addNewItem(itemToDrop);
	if (tryDrop && slot > 0 && slot < inventoryArray.Num())
	{
		removeItem(slot);
	}
	else if(!tryDrop)
	{
		newLootBag->Destroy();
	}
}