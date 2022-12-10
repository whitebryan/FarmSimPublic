// Fill out your copyright notice in the Description page of Project Settings.


#include "FishingMiniGame.h"
#include "Kismet/KismetSystemLibrary.h" 
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AFishingMiniGame::AFishingMiniGame()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	switch (miniGameQuality)
	{
	case FishingDifficulty::Easy:
		speedModifier = 0.25f;
		break;
	case FishingDifficulty::Normal:
		speedModifier = 0.35f;
		break;
	case FishingDifficulty::Hard:
		speedModifier = 0.55f;
		quantityModifier += 1;
		break;
	default:
		break;
	}
}

// Called when the game starts or when spawned
void AFishingMiniGame::BeginPlay()
{
	Super::BeginPlay();

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
	active = false;

	FVector movingScale = movingDecal->GetRelativeScale3D();
	FVector ringScale = movingDecal->GetRelativeScale3D();

	if(movingScale.Y <= 0.3 && movingScale.Y >= 0.24)
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
			FName fishName = rowNames[UKismetMathLibrary::RandomIntegerInRange(0, (rowNames.Num() - 1))];
			FInvTableItem* fishToGive = curTable->FindRow<FInvTableItem>(fishName, FString(""));

			specificFish.name = fishName;
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

void AFishingMiniGame::activate()
{
	active = true;
}

void AFishingMiniGame::finished()
{
	Destroy();
}