// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "Engine/Texture2D.h"
#include "ToolItem.generated.h"

UENUM(Blueprintable, BlueprintType)
enum ToolTier
{
	Starter = 0 UMETA(DisplayName = "Starter"),
	Bronze = 1 UMETA(DisplayName = "Bronze"),
	Silver = 2 UMETA(DisplayName = "Silver"),
	Gold = 3 UMETA(DisplayName = "Gold"),
	Diamond = 4 UMETA(DisplayName = "Diamond"),
};

USTRUCT(BlueprintType, Blueprintable)
struct FToolInvItem : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName uniqueID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName toolName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName type;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class AActor> model;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<ToolTier> toolTier;
};