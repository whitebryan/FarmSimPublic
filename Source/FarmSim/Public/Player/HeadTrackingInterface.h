// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HeadTrackingInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHeadTrackingInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FARMSIM_API IHeadTrackingInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	FVector getHeadTrackingLookOffset();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	bool bShouldActAsOffset();
};
