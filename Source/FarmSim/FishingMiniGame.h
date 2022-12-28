// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "../../Plugins/SimpleInteract/Source/SimpleInteract/Public/InteractInterface.h"
#include "InventoryItem.h"
#include "Components/DecalComponent.h" 
#include "Materials/MaterialInstanceDynamic.h"
#include "FishingMiniGame.generated.h"

UENUM(Blueprintable, BlueprintType)
enum FishingDifficulty {
	Easy UMETA(DisplayName = "Easy"),
	Normal UMETA(DisplayName = "Normal"),
	Hard UMETA(DisplayName = "Hard"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFishingDone, bool, Status, FInvItem, FishCaught);

UCLASS()
class FARMSIM_API AFishingMiniGame : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFishingMiniGame();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (Tooltip = "If set this minigame will ONLY give this fish"))
	FInvItem specificFish;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (Tooltip = "Determines the speed and types of fish recieved from this minigame. Lower difficulties have a low chance to get a better quality fish but a higher difficulty can't go lower"))
	TEnumAsByte<FishingDifficulty> miniGameQuality = FishingDifficulty::Easy;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (Tooltip = "Customizable Difficulty settings, String to float, Keys/Meaning EasySpd/speedMod, NormalSpd/speedMod, HardSpd/speedMod, NormalQnt/extraAmt, HardQnt/extraAmt oneUp/chance twoUp/chance"))
	TMap<FString, float> difficultySettings;
	float oneTierUpChance = 0.15f;
	float twoTierUpChance = 0.05f;
	float speedModifier = 0.25f;
	int quantityModifier = 0;

	UPROPERTY(BlueprintReadWrite)
	UDecalComponent* movingDecal;
	UPROPERTY(BlueprintReadWrite)
	UDecalComponent* ringDecal;

	UPROPERTY(BlueprintReadWrite)
	UMaterialInstanceDynamic* decalInstance;
	UPROPERTY(EditAnywhere)
	FLinearColor failedColor;
	UPROPERTY(EditAnywhere)
	FLinearColor successColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Tooltip = "Data tables seperated by difficulty"))
	TMap<FString, UDataTable*> dataTables;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (Tooltip = "Time in seconds till it starts moving"))
	float startDelay = 0.5f;
	FTimerHandle startDelayTimer;
	FTimerHandle destroyTimer;

	bool active = false;

	void activate();
	void finished();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnFishingDone fishingFinished;

	UFUNCTION(BlueprintCallable)
	void setDifficulty(FishingDifficulty diff);

	//Interact interface reciever
	void Interact(); virtual void Interact_Implementation() override;

	UFUNCTION(BlueprintImplementableEvent)
	void getDataTables();
};
