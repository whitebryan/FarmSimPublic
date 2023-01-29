// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "InventoryItem.h"
#include "Math/Color.h"
#include "../../Plugins/SimpleInteract/Source/SimpleInteract/Public/InteractInterface.h"
#include "BasePlant.generated.h"

UENUM(Blueprintable, BlueprintType)
enum GrowthStatus
{
	SettingUp UMETA(DisplayName = "SettingUp"),
	NeedsWater UMETA(DisplayName = "NeedsWater"),
	Watered UMETA(DisplayName = "Watered"),
	Grown UMETA(DisplayName = "Grown"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlantStatusChanged, GrowthStatus, newPlantStatus);

UCLASS(Blueprintable, BlueprintType)
class FARMSIM_API ABasePlant : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABasePlant();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	int curBreakPoint = 0;
	float totalSecondsGrowing = 0;

	UPROPERTY(BlueprintReadOnly)
	float wateredTimer = 0;
	UPROPERTY(BlueprintReadOnly)
	float wateredPercent = 1;
	float timePerBreakPoint = 0;

	UPROPERTY(BlueprintReadWrite)
	UStaticMeshComponent* plantMeshComp;
	
	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<GrowthStatus> curStatus = GrowthStatus::SettingUp;

	//UPROPERTY(EditAnywhere)
	//UDataTable* plantDataTable;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable)
	FPlantStatusChanged OnStatusUpdated;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ToolTip = "The the type of plant to grow"))
	UPlantItemAsset* plantToGrow;


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor wateredColor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor needsWaterColor;

	UPROPERTY()
	AActor* myPlot = nullptr;

	UFUNCTION(BlueprintCallable)
	void InitalizePlant(bool newPlant = false, AActor* newPlot = nullptr, float Timer = 0);

	UFUNCTION()
	void breakPointChange(int newBreakPoint);

	UFUNCTION(BlueprintCallable)
	void waterPlant();

	UFUNCTION(BlueprintCallable)
	void harvestPlant();

	UFUNCTION(BlueprintImplementableEvent)
	void toggleInteractibility();

	UFUNCTION()
	void modelChange();

	UFUNCTION()
	void statusChange(GrowthStatus newStatus);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void interactWithCrop(bool Status);

	GrowthStatus getCurStatus() { return curStatus; };
	float getTotalWateredTime() { return totalSecondsGrowing; };
};
