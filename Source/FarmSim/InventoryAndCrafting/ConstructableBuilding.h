// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "InventoryItem.h"
#include "InteractComponent.h"
#include "ConstructableBuilding.generated.h"

UCLASS()
class FARMSIM_API AConstructableBuilding : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AConstructableBuilding();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UInteractComponent* myInteractComp;

	UStaticMeshComponent* myMesh;

	UBoxComponent* myBoxCollider;

	UMaterialInstanceDynamic* myDynamicMat;
	UPROPERTY(EditAnywhere)
	UMaterialInterface* sourceMat;
	TMap<int, UMaterialInterface*> builtMats;
	UPROPERTY(EditAnywhere)
	FLinearColor nonBuiltColor;
	UPROPERTY(EditAnywhere)
	float nonBuiltTransparency;

	UPROPERTY(BlueprintReadOnly)
	bool bIsBuilt = false;

	UPROPERTY(EditAnywhere)
	TArray<AActor*> actorsToDestroyOnBuilt;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	URecipeAsset* buildingRecipe;

	UFUNCTION(BlueprintCallable)
	bool checkIfBuildable();
	UFUNCTION(BlueprintCallable)
	void tryBuild(bool status);
	UFUNCTION(BlueprintCallable)
	void forceBuild();

	UFUNCTION(BlueprintImplementableEvent)
	void forceHideUI();

	void changeStatus(bool bIsBuilt);
};
