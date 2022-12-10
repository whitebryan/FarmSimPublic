// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

class AFarmSimCharacter;
class UInputMappingContext;

/**
 * Base player controller
 */
UCLASS(Blueprintable, BlueprintType)
class FARMSIM_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void OnPossess(APawn* NewPawn) override;

	virtual void SetupInputComponent() override;

	virtual void BindActions(UInputMappingContext* Context);

protected:
	void SetupInputs();

	UFUNCTION()
	void MoveForwardAction(const FInputActionValue& Value);

	UFUNCTION()
	void MoveRightAction(const FInputActionValue& Value);

	UFUNCTION()
	void CameraUpAction(const FInputActionValue& Value);

	UFUNCTION()
	void CameraRightAction(const FInputActionValue& Value);

	UFUNCTION()
	void JumpAction(const FInputActionValue& Value);

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

public:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AFarmSimCharacter> PossessedCharacter = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UInputMappingContext> DefaultInputMappingContext = nullptr;
};
