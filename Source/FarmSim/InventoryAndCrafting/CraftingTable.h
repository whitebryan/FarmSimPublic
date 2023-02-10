// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InventoryComponent.h"
#include "../PlayerSaveManagerComponent.h"
#include "InventoryItem.h"
#include "CraftingTable.generated.h"

UCLASS()
class FARMSIM_API ACraftingTable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACraftingTable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	UDataTable* recipeTable;

	UPlayerSaveManagerComponent* playerSaveManager;
	UInventoryComponent* playerInv;

	TArray<UInventoryComponent*> inventories;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void craftItem(URecipeAsset* recipeToCraft, int quantity);

	UFUNCTION(BlueprintImplementableEvent)
	void toggleUI(bool IsVisible = true);

	UFUNCTION(BlueprintCallable)
	TMap<URecipeAsset*, int> generateCratableAmounts(const FName type = "General");

	UFUNCTION(BlueprintCallable)
	void Interact(bool status);

	UFUNCTION(BlueprintCallable)
	void grabChests();
};
