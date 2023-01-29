// Fill out your copyright notice in the Description page of Project Settings.


#include "Eleveator.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AElevator::AElevator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);

}

// Called when the game starts or when spawned
void AElevator::BeginPlay()
{
	Super::BeginPlay();
	TArray<UActorComponent*> meshComps;
	GetComponents(meshComps);
	for (UActorComponent* actComp : meshComps)
	{
		if (actComp->GetName() == "ElevatorBody")
		{
			elevatorBody = Cast<UStaticMeshComponent>(actComp);
			break;
		}
	}

	splineToFollow = Cast<USplineComponent>(GetComponentByClass(USplineComponent::StaticClass()));
	testMove = Cast<USceneComponent>(GetComponentByClass(USceneComponent::StaticClass()));
}

// Called every frame
void AElevator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector targetLoc = splineToFollow->GetLocationAtSplinePoint(targetFloor, ESplineCoordinateSpace::Local);

	FVector newLoc = UKismetMathLibrary::VInterpTo_Constant(elevatorBody->GetRelativeLocation(),targetLoc, GetWorld()->GetDeltaSeconds(), elevatorSpeed);
	if (UKismetMathLibrary::NearlyEqual_FloatFloat(newLoc.Z, targetLoc.Z, 2))
	{
		changeTextColorAndStatus("Stopped", targetFloor);
		toggleInteractability();
		elevatorBody->SetRelativeLocation(targetLoc);
		SetActorTickEnabled(false);
		curFloor = targetFloor;
	}
	else
	{
		elevatorBody->SetRelativeLocation(newLoc);
	}
}

void AElevator::moveToFloor(int floor)
{
	if (floor < 0 || floor >= splineToFollow->GetNumberOfSplinePoints() || floor == curFloor || IsActorTickEnabled())
	{
		return;
	}

	changeTextColorAndStatus("Moving", floor);
	toggleInteractability();

	SetActorTickEnabled(true);
	targetFloor = floor;
}

