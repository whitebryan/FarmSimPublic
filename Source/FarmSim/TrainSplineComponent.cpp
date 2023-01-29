// Fill out your copyright notice in the Description page of Project Settings.


#include "TrainSplineComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

void UTrainSplineComponent::moveAndRotateTrainPiece(UStaticMeshComponent* piecetoMove, int piecePosition)
{
	//Calculating distance along spline point from object position 
	FVector pos;
	float curDist = splineToFollow->FindInputKeyClosestToWorldLocation(objectToMove->GetComponentLocation());
	curDist = splineToFollow->GetDistanceAlongSplineAtSplineInputKey(curDist);

	curDist -= pieceOffset * piecePosition;

	if (curDist < 0)
	{
		curDist = splineToFollow->GetSplineLength() + curDist;
	}

	FRotator newRot = splineToFollow->GetRotationAtDistanceAlongSpline(curDist, ESplineCoordinateSpace::World);
	FVector newLoc = splineToFollow->GetLocationAtDistanceAlongSpline(curDist, ESplineCoordinateSpace::World);

	piecetoMove->SetWorldLocationAndRotation(newLoc, newRot);
}

void UTrainSplineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	//Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!isDelayed)
	{
		//Check if we are currently nearly at a spline point
		bool atPoint = objectToMove->GetRelativeLocation().Equals(splineToFollow->GetLocationAtSplinePoint(nextPoint, ESplineCoordinateSpace::Local), 30);

		if (atPoint)
		{
			TArray<int> keys;
			pointsToStopAt.GetKeys(keys);

			bool shouldStop = shouldEverStop && keys.Contains(nextPoint);

			if (splineToFollow->IsClosedLoop() && nextPoint >= numPoints)//Reset to 0 after a loop in a closed loop
			{
				nextPoint = 0;
			}
			else if (splineToFollow->IsClosedLoop())
			{
				++nextPoint;
			}

			if (shouldStop)//Pause if the spline is meant to stop
			{
				isDelayed = true;
				GetWorld()->GetTimerManager().SetTimer(endDelayer, this, &UFollowSplineComponent::toggleDelay, stopDelay, false);
				return;
			}
		}

		/*
		//Interp towards next point
		FVector newLoc = UKismetMathLibrary::VInterpTo_Constant(objectToMove->GetRelativeLocation(), splineToFollow->GetLocationAtSplinePoint(nextPoint, ESplineCoordinateSpace::Local), GetWorld()->GetDeltaSeconds(), curSpeed);

		FVector pos;
		float curDist = splineToFollow->FindInputKeyClosestToWorldLocation(objectToMove->GetComponentLocation());
		curDist = splineToFollow->GetDistanceAlongSplineAtSplineInputKey(curDist);
		FRotator newRot = splineToFollow->GetRotationAtDistanceAlongSpline(curDist, ESplineCoordinateSpace::World);

		objectToMove->SetWorldRotation(newRot);
		objectToMove->SetRelativeLocation(newLoc);
		*/

		TArray<int> keys;
		otherPieces.GetKeys(keys);

		UStaticMeshComponent* lastComp = objectToMove;
		for (int key : keys)
		{
			UStaticMeshComponent* curComp = *otherPieces.Find(key);

			float curDist = splineToFollow->FindInputKeyClosestToWorldLocation(curComp->GetComponentLocation());
			curDist = splineToFollow->GetDistanceAlongSplineAtSplineInputKey(curDist);
			float newDist = curDist + 150;

			FRotator keyRot = splineToFollow->GetRotationAtDistanceAlongSpline(curDist, ESplineCoordinateSpace::World);

			FVector origin;
			FVector extents;
			float radius;
			UKismetSystemLibrary::GetComponentBounds(lastComp, origin, extents, radius);

			curDist -= (key * pieceOffset);

			if (curComp != objectToMove)
			{
				curDist -= extents.X;
			}

			FVector keyLoc = UKismetMathLibrary::VInterpTo_Constant(curComp->GetComponentLocation(), splineToFollow->GetLocationAtDistanceAlongSpline(newDist, ESplineCoordinateSpace::World) ,GetWorld()->GetDeltaSeconds(), curSpeed);

			curComp->SetWorldLocation(keyLoc);
			curComp->SetWorldRotation(keyRot);

			lastComp = *otherPieces.Find(key);
		}
	}
}