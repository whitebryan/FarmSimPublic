// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerSaveGame.h"
#include "PlayerMappableInputConfig.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerSaveManagerComponent.generated.h"

UENUM(Blueprintable, BlueprintType)
enum FishSizeStatus
{
	NoChange UMETA(DisplayName = "No Change"),
	NewMin UMETA(DisplayName = "New Min Size"),
	NewMax UMETA(DisplayName = "New Max Size"),
	NewFish UMETA(DisplayName = "Brand New Fish"),
};

USTRUCT(BlueprintType, Blueprintable)
struct FFishRecordStruct
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<FishSizeStatus> record;
	UPROPERTY(BlueprintReadOnly)
	float newSize;
	UPROPERTY(BlueprintReadOnly)
	FName fishName;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadFinished, bool, status);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSaveNeeded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSaveFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNewItemDiscovered);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FARMSIM_API UPlayerSaveManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerSaveManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	UPlayerSaveGame* playerSaveGame;

	UPROPERTY(EditAnywhere)
	UPlayerMappableInputConfig* defaultConfig;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UInputMappingContext> DefaultInputMappingContext = nullptr;

	bool checkIfInUse(FKey keyToCheck);


	UPROPERTY(EditAnywhere)
	TSubclassOf<class AActor> chestClass;
	void saveChests();
	void loadChests();


	UPROPERTY(EditAnywhere)
	TSubclassOf<class AActor> growthPlotClass;

	FTimerHandle autoSaveHandle;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void saveGame(const FString& slot = "Slot1");

	UFUNCTION(BlueprintCallable)
	void LoadGame(const FString& slot = "Slot1");

	UFUNCTION(BlueprintCallable)
	void saveKeybinds(const FString& slot = "Slot1");

	UFUNCTION(BlueprintCallable)
	void loadKeybinds();

	UFUNCTION(BlueprintCallable)
	bool rebindAction(FName mappingToChange, FKey newKey);

	UFUNCTION(BlueprintCallable)
	void resetKeyBinds();

	UFUNCTION(BlueprintCallable)
	void saveActors();

	UFUNCTION(BlueprintCallable)
	void loadActors();

	UFUNCTION(BlueprintCallable)
	void saveDLSSMode(UDLSSMode newMode);

	UFUNCTION(BlueprintCallable)
	void changeAutoSaveTimer(int newTime);
	UFUNCTION(BlueprintCallable)
	void saveSeasonTimeWeather(const FName curWeather, float time, float season, int daysIntoSeason, int dayOfTheWeek);

	FAsyncSaveGameToSlotDelegate saveFinished;
	UPROPERTY(BlueprintAssignable)
	FOnLoadFinished loadFinished;
	UPROPERTY(BlueprintAssignable)
	FOnSaveNeeded SaveRequested;
	UPROPERTY(BlueprintAssignable)
	FOnNewItemDiscovered newItemDiscovered;
	UPROPERTY(BlueprintAssignable)
	FOnSaveFinished bpSaveFinishedDelegate;

	UPROPERTY(BlueprintReadWrite)
	int autoSaveTime = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<URecipeAsset*> learnedRecipes;
	void addRecipe(URecipeAsset* newRecipe);

	TArray<FName> constructedBuildings;
	UFUNCTION(BlueprintCallable)
	void saveBuildingStatus(const FName buildingName);


	TArray<FVector> brokenPieces;
	UFUNCTION(BlueprintCallable)
	void saveBrokenPiece(FVector loc);

	UFUNCTION(BlueprintCallable)
	FFishRecordStruct fishCaught(const FName uniqueID, float size, FName fishName);

	UPROPERTY(BlueprintReadOnly)
	TMap<FName, FFishStatSaveStruct> fishStats;

	UPROPERTY(BlueprintReadOnly)
	TArray<UItemAsset*> discoveredItems;

	UFUNCTION(BlueprintCallable)
	void tryDiscoverItem(UItemAsset* itemToDiscover);

	void passDelegateToBP(const FString& saveString, const int32 saveInt, bool saveDone);


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<TEnumAsByte<ClothesCategory>, FClothesSaveStruct> playerClothes;

	UPROPERTY(BlueprintReadOnly)
	FLinearColor skinColor;

	UFUNCTION(BlueprintCallable)
	void addClothingPiece(UModularClothingAsset* pieceToAdd, FLinearColor color);

	UFUNCTION(BlueprintCallable)
	void saveNPCConversationStatus(FString npcName, int curConvo);

	UFUNCTION(BlueprintCallable)
	void saveQuestStatus(FQuest quest);
};
