// Fill out your copyright notice in the Description page of Project Settings.


#include "FishingMiniGame.h"
#include "Kismet/KismetSystemLibrary.h" 
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AFishingMiniGame::AFishingMiniGame()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	difficultySettings.Add("EasySpd", 0.45);
	difficultySettings.Add("NormalSpd", 0.55);
	difficultySettings.Add("NormalQnt", 0);
	difficultySettings.Add("HardSpd", 0.75);
	difficultySettings.Add("HardQnt", 1);
	difficultySettings.Add("oneUp", 0.15);
	difficultySettings.Add("twoUp", 0.05);
}

// Called when the game starts or when spawned
void AFishingMiniGame::BeginPlay()
{
	Super::BeginPlay();

	getDataTables();

	oneTierUpChance = difficultySettings["oneUp"];
	twoTierUpChance = difficultySettings["twoUp"];

	if (startDelay > 0)
	{
		GetWorldTimerManager().SetTimer(startDelayTimer, this, &AFishingMiniGame::activate, startDelay, false); 
	}
	else
	{
		active = true;
	}
}

// Called every frame
void AFishingMiniGame::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!active)
	{
		return;
	}

	FVector curScale = movingDecal->GetRelativeScale3D();
	
	//Check if the ring has reached the fail state if not keep scaling it down
	if (curScale.Y <= 0.1 && curScale.Z <= 0.1)
	{
		fishingFinished.Broadcast(false, specificFish);
		active = false;

		decalInstance->SetVectorParameterValue("Color", failedColor);

		GetWorldTimerManager().SetTimer(destroyTimer, this, &AFishingMiniGame::finished, 0.3f, false);
	}
	else
	{
		curScale.Z -= DeltaTime * speedModifier;
		curScale.Y -= DeltaTime * speedModifier;

		movingDecal->SetRelativeScale3D(curScale);
	}
}

//When interacted with check if the scale is in the success range and then give a fish
void AFishingMiniGame::Interact_Implementation()
{
	getDataTables();

	active = false;

	FVector movingScale = movingDecal->GetRelativeScale3D();
	FVector ringScale = movingDecal->GetRelativeScale3D();

	if(movingScale.Y <= 0.33 && movingScale.Y >= 0.22)
	{
		decalInstance->SetVectorParameterValue("Color", successColor);
		GetWorldTimerManager().SetTimer(destroyTimer, this, &AFishingMiniGame::finished, 0.3f, false);

		if (specificFish.name != "Empty")
		{
			fishingFinished.Broadcast(true, specificFish);
		}
		else
		{
			UDataTable* curTable;

			float tableChance = UKismetMathLibrary::RandomFloatInRange(0, 1);

			switch (miniGameQuality)
			{
			case FishingDifficulty::Easy:
				if (tableChance >= 0.95)
				{
					curTable = dataTables["Hard"];
				}
				else if(tableChance >= 0.85)
				{
					curTable = dataTables["Normal"];
				}
				else
				{
					curTable = dataTables["Easy"];
				}
				break;
			case FishingDifficulty::Normal:
				if (tableChance >= 0.95)
				{
					curTable = dataTables["Hard"];
				}
				else
				{
					curTable = dataTables["Normal"];
				}
				break;
			case FishingDifficulty::Hard:
				curTable = dataTables["Hard"];
				break;
			default:
				curTable = dataTables["Easy"];
				break;
			}



			TArray<FName> rowNames = curTable->GetRowNames();
			FName fishID = rowNames[UKismetMathLibrary::RandomIntegerInRange(0, (rowNames.Num() - 1))];
			FInvTableItem* fishToGive = curTable->FindRow<FInvTableItem>(fishID, FString(""));

			specificFish.uniqueID = fishID;
			specificFish.name = fishToGive->name;
			specificFish.description = fishToGive->description;
			specificFish.icon = fishToGive->icon;
			specificFish.type = fishToGive->type;
			specificFish.quantity = 1 + quantityModifier;

			fishingFinished.Broadcast(true, specificFish);
		}
	}
	else
	{
		decalInstance->SetVectorParameterValue("Color", failedColor);
		GetWorldTimerManager().SetTimer(destroyTimer, this, &AFishingMiniGame::finished, 0.3f, false);
		fishingFinished.Broadcast(false, FInvItem());
	}
}

void AFishingMiniGame::setDifficulty(FishingDifficulty diff)
{
	if(difficultySettings.Num() == 0)
		return;

	switch (diff)
	{
	case FishingDifficulty::Normal:
		miniGameQuality = FishingDifficulty::Normal;
		speedModifier = difficultySettings["NormalSpd"];
		quantityModifier = difficultySettings["NormalQnt"];
		break;
	case FishingDifficulty::Hard:
		miniGameQuality = FishingDifficulty::Hard;
		speedModifier = difficultySettings["HardSpd"];
		quantityModifier = difficultySettings["HardQnt"];
		break;
	default:
		miniGameQuality = FishingDifficulty::Easy;
		speedModifier = difficultySettings["EasySpd"];
		break;
	}

	difficultySettings.Empty();
}

void AFishingMiniGame::activate()
{
	active = true;
}

void AFishingMiniGame::finished()
{
	Destroy();
}