// Fill out your copyright notice in the Description page of Project Settings.


#include "DestroyablePiece.h"
#include "Kismet/GameplayStatics.h"
#include "FarmSimCharacter.h"
#include "PlayerSaveManagerComponent.h"

void ADestroyablePiece::BeginPlay()
{
	Super::BeginPlay();

	switch (toolRequired)
	{
	case PlayerToolStatus::AxeOut:
		toolType = "Axe";
		break;
	case PlayerToolStatus::PickaxeOut:
		toolType = "Pickaxe";
		break;
	case PlayerToolStatus::ShovelOut:
		toolType = "Shovel";
		break;
	default:
		break;
	}
}

void ADestroyablePiece::Tick(float DeltaTime)
{
}

void ADestroyablePiece::tryHarvest()
{
	AFarmSimCharacter* player = Cast<AFarmSimCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (player->getEquippeddTool() == toolRequired)
	{
		UToolItemAsset* playerTool = player->grabTool((FName)toolTierToString());
		if (playerTool->toolTier >= toolTierRequired)
		{
			movePlayerIntoPosition();
			player->toolUsed = true;
			Cast<UPlayerSaveManagerComponent>(player->GetComponentByClass(UPlayerSaveManagerComponent::StaticClass()))->saveBrokenPiece(GetActorLocation());

			GetWorldTimerManager().SetTimer(meshChangeTimer, this, &ADestroyablePiece::breakPiece, harvestedMeshChangeDelay);
		}
		else
		{
			FString returnMessage = "You require a ";
			returnMessage += toolTierToString() + " tier tool to harvest this";
			player->displayNotification(returnMessage, 2);
			return;
		}
	}
	else
	{
		FString returnMessage = "his requires a " + toolType + " to harvest";
		player->displayNotification(returnMessage, 2);
	}
}

void ADestroyablePiece::changeStatus(bool harvestable)
{
}

void ADestroyablePiece::breakPiece()
{
	boxCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	meshComponent->SetVisibility(false);
	meshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

FString ADestroyablePiece::toolTierToString()
{
	switch (toolTierRequired)
	{
	case ToolTier::Starter:
		return "Starter";
	case ToolTier::Bronze:
		return "Bronze";
	case ToolTier::Silver:
		return "Silver";
	case ToolTier::Gold:
		return "Gold";
	case ToolTier::Diamond:
		return "Diamond";
	default:
		return "Error";
		break;
	}
}

