// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../ToolItem.h"
#include "../PlayerStatus.h"
#include "InventoryItem.h"
#include "../../Plugins/SimpleInteract/Source/SimpleInteract/Public/InteractInterface.h"
#include "Components/BoxComponent.h"
#include "InteractComponent.h"
#include "Harvestable.generated.h"

UCLASS()
class FARMSIM_API AHarvestable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHarvestable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Item properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UHarvestableItemAsset* itemToHarvest;

	FName toolType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* boxCollider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* meshComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float harvestedMeshChangeDelay = 1.5f;

	UPROPERTY(BlueprintReadOnly)
	float respawnTimer = 0;
	FTimerHandle meshChangeTimer;

	//Used to get display name from enums for return messages
	virtual FString toolTierToString();
	virtual FString toolTypeToString();
	void delayedMeshChange();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	virtual void tryHarvest();
	virtual void changeStatus(bool harvestable);

	UFUNCTION(BlueprintCallable)
	void initHarvestable(float newTime);

	float getCurTimeTillRespawn();

	UPROPERTY(BlueprintReadOnly)
	UInteractComponent* myInteractComp;
};
