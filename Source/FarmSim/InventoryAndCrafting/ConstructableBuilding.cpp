// Fill out your copyright notice in the Description page of Project Settings.


#include "ConstructableBuilding.h"
#include "InventoryComponent.h"
#include "CraftingTable.h"
#include "../Player/FarmSimCharacter.h"


// Sets default values
AConstructableBuilding::AConstructableBuilding()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AConstructableBuilding::BeginPlay()
{
	Super::BeginPlay();
	
	myInteractComp = Cast<UInteractComponent>(GetComponentByClass(UInteractComponent::StaticClass()));
	myInteractComp->OnInteract.AddDynamic(this, &AConstructableBuilding::tryBuild);
	myBoxCollider = Cast<UBoxComponent>(GetComponentByClass(UBoxComponent::StaticClass()));
	myMesh = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass()));

	builtMats.Add(0, myMesh->GetMaterial(0));
	myDynamicMat = myMesh->CreateDynamicMaterialInstance(0, sourceMat);

	for (int i = 1; i < myMesh->GetNumMaterials(); ++i)
	{
		builtMats.Add(i, myMesh->GetMaterial(i));
		myMesh->SetMaterial(i, myDynamicMat);
	}

	changeStatus(false);
}

// Called every frame
void AConstructableBuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//Just used for when loading to not drain resources
void AConstructableBuilding::forceBuild()
{
	changeStatus(true);
}

bool AConstructableBuilding::checkIfBuildable()
{
	TArray<AActor*> craftingTables;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACraftingTable::StaticClass(), craftingTables);

	TMap<URecipeAsset*, int> craftable;

	if (craftingTables.Num() <= 0)
	{
		return false;
	}
	else
	{
		ACraftingTable* curCraftingTable = Cast<ACraftingTable>(craftingTables[0]);

		curCraftingTable->grabChests();

		//Use found table to check if we can buildd this building
		craftable = curCraftingTable->generateCratableAmounts("Building");

		TArray<URecipeAsset*> keys;
		craftable.GetKeys(keys);


		AFarmSimCharacter* curPlayer = Cast<AFarmSimCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

		if (keys.Contains(buildingRecipe))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

void AConstructableBuilding::tryBuild(bool status)
{
	//Find a crafting table 
	TArray<AActor*> craftingTables;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACraftingTable::StaticClass(), craftingTables);

	TMap<URecipeAsset*, int> craftable;

	if (craftingTables.Num() <= 0)
	{
		return;
	}
	else
	{


		ACraftingTable* curCraftingTable = Cast<ACraftingTable>(craftingTables[0]);

		curCraftingTable->grabChests();

		//Use found table to check if we can buildd this building
		craftable = curCraftingTable->generateCratableAmounts("Building");

		TArray<URecipeAsset*> keys;
		craftable.GetKeys(keys);


		AFarmSimCharacter* curPlayer = Cast<AFarmSimCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

		if (keys.Contains(buildingRecipe))//We can so remove resources, make the building, and save its staus
		{
			curCraftingTable->craftItem(buildingRecipe, 1);

			changeStatus(true);

			UPlayerSaveManagerComponent* saveManager = Cast<UPlayerSaveManagerComponent>(curPlayer->GetComponentByClass(UPlayerSaveManagerComponent::StaticClass()));
			saveManager->saveBuildingStatus(buildingRecipe->name);
		}
		else//We cant so inform the player
		{
			curPlayer->displayNotification("You do not have the materials needed to build this.");
		}
	}
}

void AConstructableBuilding::changeStatus(bool bNewStatus)
{
	if (bNewStatus)
	{
		myBoxCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		myMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		
		TArray<int> keys;
		builtMats.GetKeys(keys);
		for (int i = 0; i < keys.Num(); ++i)
		{
			myMesh->SetMaterial(i, builtMats[i]);
		}

		for (int i = 0; i < actorsToDestroyOnBuilt.Num(); ++i)
		{
			actorsToDestroyOnBuilt[i]->Destroy();
		}

		forceHideUI();
		bIsBuilt = true;
	}
	else
	{
		myBoxCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		myMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		myDynamicMat->SetVectorParameterValue("Color", nonBuiltColor);
		myDynamicMat->SetScalarParameterValue("Transparency", nonBuiltTransparency);
	}
}
