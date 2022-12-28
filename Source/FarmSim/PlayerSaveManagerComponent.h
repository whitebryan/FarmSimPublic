// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerSaveGame.h"
#include "PlayerMappableInputConfig.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerSaveManagerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadFinished, bool, status);
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


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void saveGame(FString slot = "Slot1");

	UFUNCTION(BlueprintCallable)
	void loadGame(FString slot = "Slot1");

	UFUNCTION(BlueprintCallable)
	void saveKeybinds(FString slot = "Slot1");

	UFUNCTION(BlueprintCallable)
	void loadKeybinds();

	UFUNCTION(BlueprintCallable)
	bool rebindAction(FName mappingToChange, FKey newKey);

	UFUNCTION(BlueprintCallable)
	void resetKeyBinds();


	FAsyncSaveGameToSlotDelegate saveFinished;
	FOnLoadFinished loadFinished;
};
