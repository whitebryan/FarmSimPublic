// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Harvestable.h"
#include "../PlayerSaveGame.h"
#include "HarvestablePlacer.generated.h"

UCLASS()
class FARMSIM_API AHarvestablePlacer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHarvestablePlacer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
	int curPlaced = 0;

	FVector origin = FVector(0,0,0);
	FVector myExtents = FVector(0,0,0);

	FTimerHandle TrySpawnHarvestableTimer;

	void trySpawnHarvestable();
	void spawnChildActor(FVector childLocation, FRotator childRot, float spawnInProg = -1);

public:	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AHarvestable> harvestableTypeToPlace;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EPhysicalSurface> requiredPhysSurface = EPhysicalSurface::SurfaceType_Default;

	UPROPERTY(EditAnywhere)
	int rayDownLength = 300;

	UPROPERTY(EditAnywhere)
	float zOffset = 0;
	UPROPERTY(EditAnywhere)
	FRotator rotOffset = FRotator(0,0,0);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int maxPlacedInZone = 10;

	UPROPERTY(EditAnywhere)
	float chanceToSpawnPerTick = 0.3f;

	UPROPERTY(EditAnywhere)
	bool bShouldUseNormal = true;

	UPROPERTY(EditAnywhere)
	float spawnTimer = 2;

	UFUNCTION()
	void harvestableDestroyed(AActor* DestroyedActor);

	UFUNCTION()
	void reloadHarvestables(FHarvestableSaveStruct spawnedHarvestables);

	UFUNCTION()
	FHarvestableSaveStruct generateSpawnArray();

};
