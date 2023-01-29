// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Harvestable.h"
#include "ToolItem.h"
#include "DestroyablePiece.generated.h"

/**
 * 
 */
UCLASS()
class FARMSIM_API ADestroyablePiece : public AHarvestable
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void tryHarvest() override;
	void breakPiece();
	virtual void changeStatus(bool harvestable) override;

	virtual FString toolTierToString() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly);
	TEnumAsByte<PlayerToolStatus> toolRequired = PlayerToolStatus::PickaxeOut;

	FString toolType = "Pickaxe";

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<ToolTier> toolTierRequired = ToolTier::Starter;
};
