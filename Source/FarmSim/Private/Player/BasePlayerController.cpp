// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerController.h"
#include "FarmSimCharacter.h"

void ABasePlayerController::SprintAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		Cast<AFarmSimCharacter>(PossessedCharacter)->SprintAction(Value.GetMagnitude());
	}
}

void ABasePlayerController::WalkAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		Cast<AFarmSimCharacter>(PossessedCharacter)->WalkAction(Value.GetMagnitude());
	}
}

void ABasePlayerController::InteractAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		Cast<AFarmSimCharacter>(PossessedCharacter)->InteractAction();
	}
}

void ABasePlayerController::UseToolAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		Cast<AFarmSimCharacter>(PossessedCharacter)->UseToolAction();
	}
}

void ABasePlayerController::ScrollItemsAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		Cast<AFarmSimCharacter>(PossessedCharacter)->ScrollItemsAction(Value.GetMagnitude());
	}
}

void ABasePlayerController::EscMenuAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		Cast<AFarmSimCharacter>(PossessedCharacter)->EscMenuAction();
	}
}

void ABasePlayerController::OpenInventoryAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		Cast<AFarmSimCharacter>(PossessedCharacter)->toggleMenuUI(true, "Inventory", true);
	}
}

void ABasePlayerController::EquipPickaxeAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		Cast<AFarmSimCharacter>(PossessedCharacter)->changeEquippedTool(FGameplayTag::RequestGameplayTag("PlayerToolStatus.Pickaxe"));
	}
}

void ABasePlayerController::EquipAxeAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		Cast<AFarmSimCharacter>(PossessedCharacter)->changeEquippedTool(FGameplayTag::RequestGameplayTag("PlayerToolStatus.Axe"));
	}
}

void ABasePlayerController::EquipShovelAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		Cast<AFarmSimCharacter>(PossessedCharacter)->changeEquippedTool(FGameplayTag::RequestGameplayTag("PlayerToolStatus.Shovel"));
	}
}

void ABasePlayerController::EquipWaterCanAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		Cast<AFarmSimCharacter>(PossessedCharacter)->changeEquippedTool(FGameplayTag::RequestGameplayTag("PlayerToolStatus.Watering Can"));
	}
}

void ABasePlayerController::EquipFishingRodAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		Cast<AFarmSimCharacter>(PossessedCharacter)->changeEquippedTool(FGameplayTag::RequestGameplayTag("PlayerToolStatus.Fishing Rod"));
	}
}

void ABasePlayerController::FishingCastAction(const FInputActionValue& Value)
{
	if (PossessedCharacter && Cast<AFarmSimCharacter>(PossessedCharacter)->findTagOfType(Cast<AFarmSimCharacter>(PossessedCharacter)->toolStatusTag).MatchesTagExact(FGameplayTag::RequestGameplayTag("PlayerToolStatus.Fishing Rod")))
	{
		if (Value.GetMagnitude() == 1)
		{
			Cast<AFarmSimCharacter>(PossessedCharacter)->FishingCastAction(true);
		}
		else
		{
			Cast<AFarmSimCharacter>(PossessedCharacter)->FishingCastAction(false);
		}
	}
}

void ABasePlayerController::TogglePlacementModeAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		Cast<AFarmSimCharacter>(PossessedCharacter)->togglePlacementModeAction();
	}
}

void ABasePlayerController::FishingLogAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		Cast<AFarmSimCharacter>(PossessedCharacter)->toggleMenuUI(true, "Fishing", true);
	}
}

void ABasePlayerController::GatheringLogAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		Cast<AFarmSimCharacter>(PossessedCharacter)->toggleMenuUI(true, "Gathering", true);
	}
}

void ABasePlayerController::FarmingLogAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		Cast<AFarmSimCharacter>(PossessedCharacter)->toggleMenuUI(true, "Farming", true);
	}
}

void ABasePlayerController::ClothingMenuAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		Cast<AFarmSimCharacter>(PossessedCharacter)->toggleMenuUI(true, "Clothing", true);
	}
}