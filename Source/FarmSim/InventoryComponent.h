// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryItem.h"
#include "Kismet/GameplayStatics.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInvChangedDelegate);

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

	UPROPERTY(BlueprintReadOnly)
	TArray<FInvItem> inventoryArray;

	UPROPERTY(EditAnywhere, meta = (ToolTip = "The number of rows to put in this inventory, rows are 5 columns each."))
	int inventoryRows = 1;

	UPROPERTY(EditAnywhere)
	FInvItem emptyItem;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AActor> lootBag;

public:	
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnInvChangedDelegate OnInvChanged;

	UFUNCTION(BlueprintCallable)
	bool addNewItem(FInvItem newItem);

	UFUNCTION(BlueprintCallable)
	void moveItem(int from, int to);
		
	UFUNCTION(BlueprintCallable)
	void removeItem(int slot);

	UFUNCTION(BlueprintCallable)
	bool moveToNewInvComp(int slot, UInventoryComponent* newComp);

	UFUNCTION(BlueprintCallable)
	int getItemQuantity(FName itemType);

	UFUNCTION(BlueprintCallable)
	FInvItem getItemAtSlot(int slot);

	UFUNCTION(BlueprintCallable)
	int findNextItemOfType(int startPos, int direction, FName type);

	UFUNCTION(BlueprintCallable)
	int changeQuantity(FName itemType, int quantityToChange);

	UFUNCTION(BlueprintCallable)
	bool splitStack(int slot, int newStackSize);

	UFUNCTION(BlueprintCallable)
	TArray<FInvItem> getInvArrary() { return inventoryArray; };

	UFUNCTION(BlueprintCallable)
	void createLootBag(FInvItem itemToDrop, int slot = -1);

	UFUNCTION(BlueprintCallable)
	bool isEmpty();
};
