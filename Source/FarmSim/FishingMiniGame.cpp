// Fill out your copyright notice in the Description page of Project Settings.


#include "FishingMiniGame.h"
#include "Kismet/KismetSystemLibrary.h" 
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h" 
#include "Player/FarmSimCharacter.h"

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
	UDataTable* curDataTable = getDataTable();

	if (curDataTable == nullptr)
	{
		return;
	}

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
			FString fishQuality = "Normal";
			float tableChance = UKismetMathLibrary::RandomFloatInRange(0, 1);

			switch (miniGameQuality)
			{
			case FishingDifficulty::Easy:
				if (tableChance >= (1 - oneTierUpChance))
				{
					fishQuality = "Legendary";
				}
				else if(tableChance >= (1 - twoTierUpChance))
				{
					fishQuality = "Epic";
				}
				else
				{
					fishQuality = "Normal";
				}
				break;
			case FishingDifficulty::Normal:
				if (tableChance >= (1 - oneTierUpChance))
				{
					fishQuality = "Legendary";
				}
				else
				{
					fishQuality = "Epic";
				}
				break;
			case FishingDifficulty::Hard:
				fishQuality = "Legendary";
				break;
			default:
				fishQuality = "Normal";
				break;
			}


			FSeasonWeatherReturnStruct curWorldStatus = getWeatherTimeStatus();
			TArray<FName> rowNames = curDataTable->GetRowNames();
			TArray<FName> rowNamesCopy = rowNames;

			for (FName row : rowNamesCopy)
			{
				//Probably will have to revist this if I add too many fish could get real slow
				//Split the ID of the row
				//[0] = Weather
				//[1] = Time Start
				//[2] = Time End
				//[3] = Season
				//[4] = Quality
				//[5] = Location
				//[6] = ID

				TArray<FString> splitID;
				row.ToString().ParseIntoArray(splitID, TEXT("."));

				//Go through the various parts of the id and check to make sure the fish is avaible in this current status else remove it
				if (splitID[0] != "Any" && splitID[0] != curWorldStatus.weather)
				{
					rowNames.Remove(row);
					continue;
				}
				else if(splitID[1] != "99" && curWorldStatus.hour < UKismetStringLibrary::Conv_StringToInt(splitID[1]))
				{
					rowNames.Remove(row);
					continue;
				}
				else if(splitID[2] != "99" && curWorldStatus.hour > UKismetStringLibrary::Conv_StringToInt(splitID[2]))
				{
					rowNames.Remove(row);
					continue;
				}
				else if(splitID[3] != "Any" && splitID[3] != curWorldStatus.season)
				{
					rowNames.Remove(row);
					continue;
				}
				else if(splitID[4] != fishQuality)
				{
					rowNames.Remove(row);
					continue;
				}
				else if(splitID[5] != "Any" && splitID[5] != curWorldStatus.curLocation)
				{
					rowNames.Remove(row);
					continue;
				}
			}

			if (rowNames.Num() <= 0)
			{
				decalInstance->SetVectorParameterValue("Color", failedColor);
				GetWorldTimerManager().SetTimer(destroyTimer, this, &AFishingMiniGame::finished, 0.3f, false);
				fishingFinished.Broadcast(false, FInvItem());
				return;
			}

			FName fishID = rowNames[UKismetMathLibrary::RandomIntegerInRange(0, (rowNames.Num() - 1))];
			FInvTableItem* fishToGive = curDataTable->FindRow<FInvTableItem>(fishID, FString(""));

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