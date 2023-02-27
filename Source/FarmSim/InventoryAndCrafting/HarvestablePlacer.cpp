// Fill out your copyright notice in the Description page of Project Settings.


#include "HarvestablePlacer.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AHarvestablePlacer::AHarvestablePlacer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHarvestablePlacer::BeginPlay()
{
	Super::BeginPlay();
	GetActorBounds(false, origin, myExtents);
	GetWorldTimerManager().SetTimer(TrySpawnHarvestableTimer, this, &AHarvestablePlacer::trySpawnHarvestable, spawnTimer, true);
}

void AHarvestablePlacer::trySpawnHarvestable()
{
	FActorSpawnParameters myParams;
	myParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;

	FVector spawnLoc = UKismetMathLibrary::RandomPointInBoundingBox(origin, myExtents);

	FHitResult RV_Hit(ForceInit);
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), false, this);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.AddIgnoredActor(this);

	bool hit = GetWorld()->LineTraceSingleByChannel(
		RV_Hit,
		spawnLoc,
		spawnLoc - FVector(0, 0, 150),
		ECC_Visibility,
		RV_TraceParams
	);

	if (!hit)
	{
		return;
	}


	spawnLoc = RV_Hit.ImpactPoint + posOffset;
	FRotator normalToRot = RV_Hit.ImpactNormal.Rotation();


	FRotator randomVariation = FRotator(0, UKismetMathLibrary::RandomIntegerInRange(0, 359), 0);

	FRotator spawnRot = normalToRot + rotOffset + randomVariation;

	AActor* newHarvestable = GetWorld()->SpawnActor<AHarvestable>(harvestableTypeToPlace, spawnLoc, FRotator(0, 0, 0));
	if (IsValid(newHarvestable))
	{
		if (bShouldUseNormal)
		{
			spawnRot = FRotator(spawnRot.Pitch, spawnRot.Yaw, UKismetMathLibrary::FClamp(spawnRot.Roll, 0, 20));
			Cast<UStaticMeshComponent>(newHarvestable->GetComponentByClass(UStaticMeshComponent::StaticClass()))->SetWorldRotation(spawnRot);
		}

		newHarvestable->OnDestroyed.AddDynamic(this, &AHarvestablePlacer::harvestableDestroyed);
		++curPlaced;

		if (curPlaced == maxPlacedInZone)
		{
			GetWorldTimerManager().ClearTimer(TrySpawnHarvestableTimer);
		}
	}
}

void AHarvestablePlacer::harvestableDestroyed(AActor* DestroyedActor)
{
	--curPlaced;
	GetWorldTimerManager().SetTimer(TrySpawnHarvestableTimer, this, &AHarvestablePlacer::trySpawnHarvestable, spawnTimer, true);
}