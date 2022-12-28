// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BasePlant.h"
#include "SimpleInteract/Public/InteractComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/DataTable.h"
#include "GrowthPlot.generated.h"

UCLASS()
class FARMSIM_API AGrowthPlot : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrowthPlot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//My interact component
	UInteractComponent* myInteract;
	UBoxComponent* myCollider = nullptr;

	//Initalized in Blueprint
	UPROPERTY(BlueprintReadWrite)
	FString curSeason = "none";

	UPROPERTY(EditAnywhere)
	UDataTable* plantDataTable;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Called when interacted with and plants given type
	UFUNCTION(BlueprintCallable)
	bool plantCrop(FName cropName);

	UFUNCTION()
	void changeInteractability();

	UFUNCTION(BlueprintCallable)
	bool isInUse();
};

