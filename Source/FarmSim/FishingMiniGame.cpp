// Fill out your copyright notice in the Description page of Project Settings.


#include "FishingMiniGame.h"
#include "Kismet/KismetSystemLibrary.h" 
#include "Kismet/KismetMathLibrary.h"
#include "FarmSimCharacter.h"

// Sets default values
AFishingMiniGame::AFishingMiniGame()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	difficultySettings.Add("EasySpd", 1.2);
	difficultySettings.Add("NormalSpd", 1.65);
	difficultySettings.Add("NormalQnt", 0);
	difficultySettings.Add("HardSpd", 2.35);
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
	if (curScale.Y <= 1.19f)
	{
		FInvItem failed;
		fishingFinished.Broadcast(false, failed);
		active = false;

		decalInstance->SetVectorParameterValue("Color", failedColor);

		GetWorldTimerManager().SetTimer(destroyTimer, this, &AFishingMiniGame::finished, 0.3f, false);
	}
	else
	{
		curScale.X -= DeltaTime * speedModifier;
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

	if(movingScale.Y <= 1.7f && movingScale.Y >= 1.2f)
	{
		decalInstance->SetVectorParameterValue("Color", successColor);
		GetWorldTimerManager().SetTimer(destroyTimer, this, &AFishingMiniGame::finished, 0.3f, false);

		if (IsValid(specificFish) && specificFish->name != "Empty")
		{
			FInvItem newInvItem;
			newInvItem.quantity = 1 + quantityModifier;
			newInvItem.item = specificFish;

			float fishSize = UKismetMathLibrary::RandomFloatInRange(specificFish->minLenght, specificFish->maxLength);

			AActor* player = Cast<AActor>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

			FFishRecordStruct fishStatus = Cast<UPlayerSaveManagerComponent>(player->GetComponentByClass(UPlayerSaveManagerComponent::StaticClass()))->fishCaught(specificFish->uniqueID, fishSize, specificFish->name);

			newFishingRecord.Broadcast(fishStatus);
			fishingFinished.Broadcast(true, newInvItem);
		}
		else
		{
			UDataTable* curTable;

			float tableChance = UKismetMathLibrary::RandomFloatInRange(0, 1);

			switch (miniGameQuality)
			{
			case FishingDifficulty::Easy:
				if (tableChance >= (1 - oneTierUpChance))
				{
					curTable = dataTables["Hard"];
				}
				else if(tableChance >= (1 - twoTierUpChance))
				{
					curTable = dataTables["Normal"];
				}
				else
				{
					curTable = dataTables["Easy"];
				}
				break;
			case FishingDifficulty::Normal:
				if (tableChance >= (1 - oneTierUpChance))
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

			FInvItem newInvItem;
			newInvItem.item = Cast<UItemAsset>(fishToGive->item);
			newInvItem.quantity = 1 + quantityModifier;

			UFishItemAsset* curFish = Cast<UFishItemAsset>(newInvItem.item);

			float fishSize = UKismetMathLibrary::RandomFloatInRange(curFish->minLenght, curFish->maxLength);

			AActor* player = Cast<AActor>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

			FFishRecordStruct fishStatus = Cast<UPlayerSaveManagerComponent>(player->GetComponentByClass(UPlayerSaveManagerComponent::StaticClass()))->fishCaught(newInvItem.item->uniqueID, fishSize, newInvItem.item->name);

			newFishingRecord.Broadcast(fishStatus);
			fishingFinished.Broadcast(true, newInvItem);
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