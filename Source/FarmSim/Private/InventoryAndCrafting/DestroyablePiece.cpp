// Fill out your copyright notice in the Description page of Project Settings.


#include "DestroyablePiece.h"
#include "Kismet/GameplayStatics.h"
#include "../Player/FarmSimCharacter.h"
#include "../PlayerSaveManagerComponent.h"

void ADestroyablePiece::BeginPlay()
{
	Super::BeginPlay();

	myInteractComp = Cast<UInteractComponent>(GetComponentByClass(UInteractComponent::StaticClass()));

	FString toolKey;
	FString toolTypeString;

	toolRequired.GetTagName().ToString().Split(FString("."), &toolKey, &toolTypeString);

	if (toolTypeString == "Axe")
	{
		toolType = "Axe";
	}
	else if (toolTypeString == "Pickaxe")
	{
		toolType = "Pickaxe";
	}
	else if (toolTypeString == "Pickaxe")
	{
		toolType = "Shovel";
	}
	else
	{
		toolType = "Error";
	}
}

void ADestroyablePiece::Tick(float DeltaTime)
{
}

void ADestroyablePiece::tryHarvest()
{
	AFarmSimCharacter* player = Cast<AFarmSimCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (player->findTagOfType(player->toolStatusTag).MatchesTagExact(toolRequired))
	{
		UToolItemAsset* playerTool = player->grabTool(toolRequired);
		if (playerTool->type == toolType && playerTool->toolTier >= toolTierRequired)
		{
			//Change to work with NPCs as well
			myInteractComp->moveActorIntoPlace(Cast<AActor>(player));

			player->toolUsed = true;
			Cast<UPlayerSaveManagerComponent>(player->GetComponentByClass(UPlayerSaveManagerComponent::StaticClass()))->saveBrokenPiece(GetActorLocation());

			GetWorldTimerManager().SetTimer(meshChangeTimer, this, &ADestroyablePiece::breakPiece, harvestedMeshChangeDelay);
		}
		else
		{
			FString returnMessage = "You require a ";
			returnMessage += toolTierToString() + " tier tool to destroy this";
			player->displayNotification(returnMessage, 2);
			return;
		}
	}
	else
	{
		FString returnMessage = "his requires a " + toolType.ToString() + " to destroy";
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

