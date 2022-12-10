// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
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

	UPROPERTY(BlueprintReadWrite)
	FDateTime plantedDate;

	int curBreakPoint = 0;

	UPROPERTY(BlueprintReadOnly)
	float wateredTimer = 0;
	UPROPERTY(BlueprintReadOnly)
	float wateredPercent = 1;

	UPROPERTY(BlueprintReadWrite)
	UStaticMeshComponent* plantMeshComp;
	
	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<GrowthStatus> curStatus = GrowthStatus::SettingUp;

	UPROPERTY(EditAnywhere)
	UDataTable* plantDataTable;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable)
	FPlantStatusChanged OnStatusUpdated;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ToolTip = "The name of the plant in the dataTable"))
	FName plantName = "default";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "1", ClampMax = "999", UIMin = "1", UIMax = "999", ToolTip = "Real time seconds to full growth"))
	int secondsToGrow = 99999;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "1", UIMin = "1", ToolTip = "Minimun amount harvested per harvest"))
	int harvestMin = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ToolTip = "Maximum amount to harvest per harvest"))
	int harvestMax = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "1", ClampMax = "5", UIMin = "1", UIMax = "5", ToolTip = "How many times you will need to water the plant broken into even chunks"))
	int timesToWater = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ToolTip = "The unwatered version of the plant"))
	UStaticMesh* unwateredModel;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ToolTip = "The watered but non grown version of the plant"))
	UStaticMesh* wateredGrowingModel;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ToolTip = "The grown version of the plant"))
	UStaticMesh* grownModel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor wateredColor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor needsWaterColor;

	UPROPERTY()
	AActor* myPlot = nullptr;

	UFUNCTION(BlueprintCallable)
	void InitalizePlant(FDateTime plantedTime, bool useCurTime = false);

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
};
