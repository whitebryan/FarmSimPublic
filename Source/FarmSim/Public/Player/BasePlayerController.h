// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "BaseRebindableController.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

/**
 * Base player controller
 */
UCLASS(Blueprintable, BlueprintType)
class FARMSIM_API ABasePlayerController : public ABaseRebindableController
{
	GENERATED_BODY()
protected:

	UFUNCTION()
	void SprintAction(const FInputActionValue& Value);

	UFUNCTION()
	void WalkAction(const FInputActionValue& Value);

	UFUNCTION()
	void InteractAction(const FInputActionValue& Value);

	UFUNCTION()
	void UseToolAction(const FInputActionValue& Value);

	UFUNCTION()
	void ScrollItemsAction(const FInputActionValue& Value);

	UFUNCTION()
	void EscMenuAction(const FInputActionValue& Value);

	UFUNCTION()
	void OpenInventoryAction(const FInputActionValue& Value);

	UFUNCTION()
	void EquipPickaxeAction(const FInputActionValue& Value);

	UFUNCTION()
	void EquipAxeAction(const FInputActionValue& Value);

	UFUNCTION()
	void EquipShovelAction(const FInputActionValue& Value);

	UFUNCTION()
	void EquipWaterCanAction(const FInputActionValue& Value);

	UFUNCTION()
	void EquipFishingRodAction(const FInputActionValue& Value);

	UFUNCTION()
	void FishingCastAction(const FInputActionValue& Value);

	UFUNCTION()
	void TogglePlacementModeAction(const FInputActionValue& Value);
	UFUNCTION()
	void FishingLogAction(const FInputActionValue& Value);
	UFUNCTION()
	void FarmingLogAction(const FInputActionValue& Value);
	UFUNCTION()
	void GatheringLogAction(const FInputActionValue& Value);
	UFUNCTION()
	void ClothingMenuAction(const FInputActionValue& Value);
};
