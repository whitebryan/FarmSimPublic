// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryItem.h"
#include "Kismet/GameplayStatics.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInvChangedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRowsAddedDelegate);

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FARMSIM_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "1", ClampMax = "5", UMin = "1", UMax = "5", ToolTip = "The number of rows to put in this inventory, rows are 5 columns each."))
	int inventoryRows = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "1", ClampMax = "5", UMin = "1", UMax = "5", ToolTip = "The Max number of rows that can be added to this inventory."))
	int maxInventoryRows = 5;

	UPROPERTY(EditAnywhere)
	UItemAsset* emptyItem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0", ClampMax = "2000", UMin = "0", UMax = "2000", ToolTip = "The range to add to a lootbag instead of creating a new one."))
	float mergeDist = 500;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AActor> lootBag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip ="The item needed to upgrade this inventory(must be in this specific inventory to use)"))
	UItemAsset* upgradeItem;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip = "Amount of the upgrade item needed"))
	int amtToUpgrade = 1;

public:	
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnInvChangedDelegate OnInvChanged;
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnRowsAddedDelegate OnRowsAddedd;


	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TArray<FInvItem> inventoryArray;

	UFUNCTION(BlueprintCallable)
	bool addNewRows(int numRows = 1, bool ignoreUpgradeItem = false);

	UFUNCTION(BlueprintCallable, meta = (ToolTip = "Adds as much from the stack of new item to the inventory as possible and drops the rest"))
	FAddItemStatus addNewItem(const FInvItem& newItem, bool dropIfNoneAdded = false, bool dropIfPartialAdded = true);

	UFUNCTION(BlueprintCallable)
	void addItemAtSlot(const FInvItem& newItem, int slot);

	UFUNCTION(BlueprintCallable)
	void moveItem(int from, int to);
		
	UFUNCTION(BlueprintCallable)
	void removeItem(int slot);

	UFUNCTION(BlueprintCallable)
	bool moveToNewInvComp(int slot, UInventoryComponent* newComp);

	UFUNCTION(BlueprintCallable)
	int getItemQuantity(const FName itemID);

	UFUNCTION(BlueprintCallable)
	FInvItem getItemAtSlot(int slot);

	UFUNCTION(BlueprintCallable)
	bool itemTypeExists(const FName typeToSearchFor);

	UFUNCTION(BlueprintCallable)
	int findNextItemOfType(int startPos, int direction, const FName itemType);

	UFUNCTION(BlueprintCallable)
	int changeQuantity(const FName itemID, int quantityToChange);

	UFUNCTION(BlueprintCallable)
	bool splitStack(int slot, int newStackSize);

	UFUNCTION(BlueprintCallable)
	void createLootBag(const FInvItem& itemToDrop, int slot = -1);

	UFUNCTION(BlueprintCallable)
	bool isEmpty();

	UFUNCTION(BlueprintCallable)
	const TArray<FInvItem> getInventory() { return inventoryArray; };

	UFUNCTION(BlueprintCallable)
	int getRows();

	UFUNCTION(BlueprintCallable)
	void loadInventory(TArray<FInvItem> newInv, int rowsToAdd = 0);
};
