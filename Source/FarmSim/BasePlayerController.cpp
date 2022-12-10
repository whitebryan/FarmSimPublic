// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerController.h"
#include "FarmSimCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "PlayerStatus.h"



//OnPossess, SetupInputComponent, BindActions, SetupInputs, and based model for actions from 
// https://github.com/dyanikoglu/ALS-Community
//https://github.com/dyanikoglu/ALS-Community/blob/main/LICENSE

void ABasePlayerController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);
	PossessedCharacter = Cast<AFarmSimCharacter>(NewPawn);
	SetupInputs();
}

void ABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->ClearActionEventBindings();
		EnhancedInputComponent->ClearActionValueBindings();
		EnhancedInputComponent->ClearDebugKeyBindings();

		BindActions(DefaultInputMappingContext);
	}

}

void ABasePlayerController::BindActions(UInputMappingContext* Context)
{
	if (Context)
	{
		const TArray<FEnhancedActionKeyMapping>& Mappings = Context->GetMappings();
		UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
		if (EnhancedInputComponent)
		{
			TSet<const UInputAction*> UniqueActions;
			for (const FEnhancedActionKeyMapping& Keymapping : Mappings)
			{
				UniqueActions.Add(Keymapping.Action);
			}
			for (const UInputAction* UniqueAction : UniqueActions)
			{
				EnhancedInputComponent->BindAction(UniqueAction, ETriggerEvent::Triggered, Cast<UObject>(this), UniqueAction->GetFName());
			}
		}
	}
}

void ABasePlayerController::SetupInputs()
{
	if (PossessedCharacter)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			FModifyContextOptions Options;
			Options.bForceImmediately = 1;
			Subsystem->AddMappingContext(DefaultInputMappingContext, 1, Options);
		}
	}
}




void ABasePlayerController::MoveForwardAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		PossessedCharacter->MoveForwardAction(Value.GetMagnitude());
	}
}

void ABasePlayerController::MoveRightAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		PossessedCharacter->MoveRightAction(Value.GetMagnitude());
	}
}

void ABasePlayerController::CameraUpAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		PossessedCharacter->CameraUpAction(Value.GetMagnitude());
	}
}

void ABasePlayerController::CameraRightAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		PossessedCharacter->CameraRightAction(Value.GetMagnitude());
	}
}

void ABasePlayerController::JumpAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		PossessedCharacter->JumpAction(Value.Get<bool>());
	}
}

void ABasePlayerController::SprintAction(const FInputActionValue& Value)
{
}

void ABasePlayerController::WalkAction(const FInputActionValue& Value)
{
}

void ABasePlayerController::InteractAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		PossessedCharacter->InteractAction();
	}
}

void ABasePlayerController::UseToolAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		PossessedCharacter->UseToolAction();
	}
}

void ABasePlayerController::ScrollItemsAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		PossessedCharacter->ScrollItemsAction(Value.GetMagnitude());
	}
}

void ABasePlayerController::EscMenuAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		PossessedCharacter->EscMenuAction();
	}
}

void ABasePlayerController::OpenInventoryAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		PossessedCharacter->OpenInventoryAction();
	}
}

void ABasePlayerController::EquipPickaxeAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		PossessedCharacter->changeEquippedTool(PlayerToolStatus::PickaxeOut);
	}
}

void ABasePlayerController::EquipAxeAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		PossessedCharacter->changeEquippedTool(PlayerToolStatus::AxeOut);
	}
}

void ABasePlayerController::EquipShovelAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		PossessedCharacter->changeEquippedTool(PlayerToolStatus::ShovelOut);
	}
}

void ABasePlayerController::EquipWaterCanAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		PossessedCharacter->changeEquippedTool(PlayerToolStatus::WateringCanOut);
	}
}

void ABasePlayerController::EquipFishingRodAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		PossessedCharacter->changeEquippedTool(PlayerToolStatus::FishingRodOut);
	}
}

void ABasePlayerController::FishingCastAction(const FInputActionValue& Value)
{
	if (PossessedCharacter && PossessedCharacter->getEquippeddTool() == PlayerToolStatus::FishingRodOut)
	{
		if (Value.GetMagnitude() == 1)
		{
			PossessedCharacter->FishingCastAction(true);
		}
		else
		{
			PossessedCharacter->FishingCastAction(false);
		}
	}
}