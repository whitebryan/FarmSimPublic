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
	GetActorBounds(true, origin, myExtents);


	if(!UGameplayStatics::DoesSaveGameExist("Slot1", 0))
	{
		GetWorldTimerManager().SetTimer(TrySpawnHarvestableTimer, this, &AHarvestablePlacer::trySpawnHarvestable, spawnTimer, true);
	}
}

void AHarvestablePlacer::trySpawnHarvestable()
{
	if(curPlaced >= maxPlacedInZone) return;

	FVector spawnLoc = UKismetMathLibrary::RandomPointInBoundingBox(origin, (myExtents * .65));

	FHitResult RV_Hit(ForceInit);
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), false, this);
	RV_TraceParams.bReturnPhysicalMaterial = true;

	bool hit = GetWorld()->LineTraceSingleByChannel(
		RV_Hit,
		spawnLoc,
		spawnLoc - FVector(0, 0, rayDownLength),
		ECC_WorldStatic,
		RV_TraceParams
	);

	if (!hit)
	{
		return;
	}
	else if (requiredPhysSurface != EPhysicalSurface::SurfaceType_Default)
	{
		EPhysicalSurface surfaceHit = UGameplayStatics::GetSurfaceType(RV_Hit);
	
		if (surfaceHit != requiredPhysSurface)
		{
			return;
		}
	}

	spawnLoc = RV_Hit.ImpactPoint;
	spawnLoc.Z += zOffset;
	FRotator normalToRot = RV_Hit.ImpactNormal.Rotation();

	FRotator randomVariation = FRotator(0, UKismetMathLibrary::RandomIntegerInRange(0, 359), 0);

	FRotator spawnRot = normalToRot + rotOffset + randomVariation;
	if (bShouldUseNormal)
	{
		spawnRot = FRotator(spawnRot.Pitch - 90, spawnRot.Yaw, spawnRot.Roll);
	}

	spawnChildActor(spawnLoc, spawnRot);
}

void AHarvestablePlacer::spawnChildActor(FVector childLocation, FRotator childRot, float spawnInProg)
{
	UChildActorComponent* newChildActor = NewObject<UChildActorComponent>(this, UChildActorComponent::StaticClass());
	newChildActor->SetChildActorClass(harvestableTypeToPlace);
	newChildActor->Mobility = EComponentMobility::Movable;
	newChildActor->SetVisibility(true);
	newChildActor->CreationMethod = EComponentCreationMethod::Instance;

	FinishAndRegisterComponent(newChildActor);


	if (IsValid(newChildActor))
	{
		if (bShouldUseNormal)
		{
			Cast<UStaticMeshComponent>(newChildActor->GetChildActor()->GetComponentByClass(UStaticMeshComponent::StaticClass()))->SetWorldRotation(childRot);
		}

		AHarvestable* curHarvestable = Cast<AHarvestable>(newChildActor->GetChildActor());
		if (!curHarvestable->itemToHarvest->respawnAble)
		{
			newChildActor->GetChildActor()->OnDestroyed.AddDynamic(this, &AHarvestablePlacer::harvestableDestroyed);
		}

		if (spawnInProg != -1)
		{
			curHarvestable->initHarvestable(spawnInProg);
		}

		newChildActor->GetChildActor()->SetActorLocation(childLocation);

		++curPlaced;

		if (curPlaced == maxPlacedInZone && TrySpawnHarvestableTimer.IsValid())
		{
			GetWorldTimerManager().ClearTimer(TrySpawnHarvestableTimer);
		}
	}
}


void AHarvestablePlacer::harvestableDestroyed(AActor* DestroyedActor)
{
	--curPlaced;

	TArray<UChildActorComponent*> childrenActors;
	GetComponents<UChildActorComponent>(childrenActors);

	for (int i = 0; i < childrenActors.Num(); ++i)
	{
		if (!IsValid(childrenActors[i]->GetChildActor()))
		{
			childrenActors[i]->DestroyComponent();
		}
	}

	GetWorldTimerManager().SetTimer(TrySpawnHarvestableTimer, this, &AHarvestablePlacer::trySpawnHarvestable, spawnTimer, true);
}

void AHarvestablePlacer::reloadHarvestables(FHarvestableSaveStruct spawnedHarvestables)
{
	for (int i = 0; i < spawnedHarvestables.childTransforms.Num(); ++i)
	{
		spawnChildActor(spawnedHarvestables.childTransforms[i].GetLocation(), 
			spawnedHarvestables.childTransforms[i].GetRotation().Rotator(), 
			spawnedHarvestables.childTimers[spawnedHarvestables.childTransforms[i].GetLocation()]);
	}

	GetWorldTimerManager().SetTimer(TrySpawnHarvestableTimer, this, &AHarvestablePlacer::trySpawnHarvestable, spawnTimer, true);
}

FHarvestableSaveStruct AHarvestablePlacer::generateSpawnArray()
{
	FHarvestableSaveStruct result;
	TArray<FTransform> childTransformArray;
	TMap<FVector, float> childTimers;

	TArray<UChildActorComponent*> childActors;
	GetComponents<UChildActorComponent>(childActors);

	for (UChildActorComponent* curChild : childActors)
	{
		if (IsValid(curChild))
		{
			FTransform childTransform;
			childTransform.SetLocation(curChild->GetChildActor()->GetActorLocation());
			childTransform.SetRotation(Cast<UStaticMeshComponent>(curChild->GetChildActor()->GetComponentByClass(UStaticMeshComponent::StaticClass()))->GetComponentRotation().Quaternion());
			childTransformArray.Add(childTransform);

			float timeLeft = Cast<AHarvestable>(curChild->GetChildActor())->getCurTimeTillRespawn();
			if (timeLeft == 0) timeLeft = -1; //to make loading easier if it doesnt need to init when reloading set it to -1

			childTimers.Add(childTransform.GetLocation(), timeLeft);
		}
	}

	result.childTransforms = childTransformArray;
	result.childTimers = childTimers;

	return result;
}


