// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ItemHighlightInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UItemHighlightInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FARMSIM_API IItemHighlightInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Highlight")
	void ChangeHighlight(bool active);
};
