// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "ToolItem.h"
#include "PlayerStatus.h"
#include "../../Plugins/SimpleInteract/Source/SimpleInteract/Public/InteractInterface.h"
#include "Components/BoxComponent.h"
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
	FName itemName = "default";

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UDataTable* harvestablesTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int harvestAmount = 1;


	//Tool Requirements
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool requiresTool = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName toolType = "default";
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<PlayerToolStatus> requiredToolStatus = PlayerToolStatus::PickaxeOut;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<ToolTier> toolLevel = ToolTier::Bronze;

	//Respawn variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool respawnAble = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int respawnTime = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* boxCollider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* meshComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMesh* harvestedModel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* harvestableModel;

	FTimerHandle respawnTimer;

	//Used to get display name from enums for return messages
	FString toolTierToString();
	FString toolTypeToString();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void tryHarvest();

	UFUNCTION()
	void respawn();

	void changeStatus(bool harvestable);
};
