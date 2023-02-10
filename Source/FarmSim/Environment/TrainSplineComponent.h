// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FollowSplineComponent.h"
#include "TrainSplineComponent.generated.h"

/**
 * 
 */

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FARMSIM_API UTrainSplineComponent : public UFollowSplineComponent
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void moveAndRotateTrainPiece(UStaticMeshComponent* piecetoMove, int piecePosition);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip = "The distance each piece should be offset by each other"))
	float pieceOffset = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip = "Spline points that the train shoudl stop at."))
	TMap<int, FName> pointsToStopAt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int, UStaticMeshComponent*> otherPieces;

protected:

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
