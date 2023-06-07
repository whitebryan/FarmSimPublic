// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BasePlant.h"
#include "BaseSprinkler.generated.h"

UCLASS()
class FARMSIM_API ABaseSprinkler : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseSprinkler();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TArray<ABasePlant*> plantsToWater;

	UPROPERTY(BlueprintReadOnly)
	AActor* playerChar = nullptr;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, meta = (ToolTip = "Any plant in this range will be instantly watered"))
	float sprinklerRange = 500;

	bool checkingForPlants = 0;

	UFUNCTION()
	void PlantStatusChanged(GrowthStatus newStatus);

	void checkForPlants();
};
