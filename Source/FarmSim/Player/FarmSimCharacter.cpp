// Copyright Epic Games, Inc. All Rights Reserved.

#include "FarmSimCharacter.h"
#include "SimpleInteract/Public/InteractInterface.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "../Farming/GrowthPlot.h"
#include "../Farming/BasePlant.h"
#include "../ItemHighlightInterface.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Chaos/ChaosEngineInterface.h"
#include "../FishingMiniGame.h"

#define SurfaceTypeGround EPhysicalSurface::SurfaceType1
#define SurfaceTypeWater EPhysicalSurface::SurfaceType2

//////////////////////////////////////////////////////////////////////////
// AFarmSimCharacter

AFarmSimCharacter::AFarmSimCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	//myInventoryComp = Cast<UInventoryComponent>(GetComponentByClass(UInventoryComponent::StaticClass()));

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AFarmSimCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld())//Create a growth plot and grab its Z extent for use later in spawning
	{
		AActor* newPlot = GetWorld()->SpawnActor<AActor>(growthPlotActor, FVector(-200, 0, 0), UKismetMathLibrary::MakeRotator(0, 0, 0));

		if (IsValid(newPlot))
		{
			FVector MyOrigin;
			FVector MyExtent;
			newPlot->GetActorBounds(true, MyOrigin, MyExtent, false);

			growthPlotZExtent = MyExtent.Z;

			newPlot->Destroy();
		}
	}
	UCapsuleComponent* myCapsule = Cast<UCapsuleComponent>(GetComponentByClass(UCapsuleComponent::StaticClass()));
	myCapsule->OnComponentBeginOverlap.AddDynamic(this, &AFarmSimCharacter::OnOverlapBegin);
	myCapsule->OnComponentEndOverlap.AddDynamic(this, &AFarmSimCharacter::OnOverlapEnd);

	mySaveManager = Cast<UPlayerSaveManagerComponent>(GetComponentByClass(UPlayerSaveManagerComponent::StaticClass()));
	toolMesh = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("toolMesh")));
}

//
void AFarmSimCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp->ComponentHasTag("Interaction"))
	{
		if (IsValid(lastHighlighted) && lastHighlighted != OtherActor)//Disable old highlight if there is one
		{
			if (lastHighlighted->GetClass()->ImplementsInterface(UItemHighlightInterface::StaticClass()))
			{
				IItemHighlightInterface::Execute_ChangeHighlight(lastHighlighted, false);
			}
		}

		//Add new interacble to list and update UI
		interactActorComp = Cast<UInteractComponent>(OtherActor->GetComponentByClass(UInteractComponent::StaticClass()));
		otherInteractComps.Add(interactActorComp);
		objectToTrack = OtherActor;
		updateInteractPrompt();

		//Enable new highlight
		if (OtherActor->GetClass()->ImplementsInterface(UItemHighlightInterface::StaticClass()))
		{
			lastHighlighted = OtherActor;
			IItemHighlightInterface::Execute_ChangeHighlight(lastHighlighted, true);
		}
		
	}

	if (OtherComp->ComponentHasTag("headTracking"))
	{
		objectToTrack = OtherActor;
	}
}

void AFarmSimCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherComp->ComponentHasTag("Interaction") && IsValid(interactActorComp))
	{
		UInteractComponent* otherComp = Cast<UInteractComponent>(OtherActor->GetComponentByClass(UInteractComponent::StaticClass()));

		if (otherComp == interactActorComp)//Check if we are looking at the current selected interactable
		{
			if (OtherActor == lastHighlighted && OtherActor->GetClass()->ImplementsInterface(UItemHighlightInterface::StaticClass()))//Disable old highlight
			{
				IItemHighlightInterface::Execute_ChangeHighlight(OtherActor, false);
				lastHighlighted = nullptr;
			}

			//Remove old actor
			otherInteractComps.Remove(interactActorComp);

			//Set new actor and highlight it if there is one and update ui, otherwise high UI
			if (otherInteractComps.Num() > 0)
			{
				interactActorComp = otherInteractComps[0];
				objectToTrack = otherInteractComps[0]->GetOwner();
				updateInteractPrompt();

				if (interactActorComp->GetOwner()->GetClass()->ImplementsInterface(UItemHighlightInterface::StaticClass()))
				{
					lastHighlighted = interactActorComp->GetOwner();
					IItemHighlightInterface::Execute_ChangeHighlight(lastHighlighted, true);
				}
			}
			else
			{
				objectToTrack = nullptr;
				interactActorComp = nullptr;
				updateInteractPrompt(false);
			}
		}
		else
		{
			otherInteractComps.Remove(otherComp);

			if (OtherActor->GetClass()->ImplementsInterface(UItemHighlightInterface::StaticClass()))//Disable old highlight
			{
				IItemHighlightInterface::Execute_ChangeHighlight(OtherActor, false);
			}
		}
	}


	if (OtherComp->ComponentHasTag("headTracking"))
	{
		objectToTrack = nullptr;
		if (otherInteractComps.Num() > 0)
		{
			interactActorComp = otherInteractComps[0];
			objectToTrack = otherInteractComps[0]->GetOwner();
			updateInteractPrompt();

			if (interactActorComp->GetOwner()->GetClass()->ImplementsInterface(UItemHighlightInterface::StaticClass()))
			{
				lastHighlighted = interactActorComp->GetOwner();
				IItemHighlightInterface::Execute_ChangeHighlight(lastHighlighted, true);
			}
		}
	}
}

void AFarmSimCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Check if any notification messages are queued
	if (!notificationQueue.IsEmpty() && curNotification == "null")
	{
		TTuple<FString, int> newNotif;
		notificationQueue.Dequeue(newNotif);

		newNotification(newNotif.Key, newNotif.Value);
		curNotification = newNotif.Key;
	}

	if (toolStatus == PlayerToolStatus::ShovelOut || toolStatus == PlayerToolStatus::WateringCanOut)//Create and show a placement preview for digging a growth plot or watering crops
	{
		FHitResult RV_Hit;
		RV_Hit = placementLineTraceDown();
		EPhysicalSurface surfaceHit = UGameplayStatics::GetSurfaceType(RV_Hit);

		FVector placementPoint = RV_Hit.ImpactPoint;
		placementPoint.Z += growthPlotZExtent;

		if (!IsValid(placementPreview) && toolStatus == PlayerToolStatus::ShovelOut)
		{
			FActorSpawnParameters myParams;
			myParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			placementPreview = GetWorld()->SpawnActor<AActor>(growthPlotPreview, placementPoint, UKismetMathLibrary::MakeRotator(0, 0, 0), myParams);
			placementPreview->SetHidden(false);
			objectToTrack = placementPreview;
		}
		else if (!IsValid(placementPreview) && toolStatus == PlayerToolStatus::WateringCanOut)
		{
			FActorSpawnParameters myParams;
			myParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			placementPreview = GetWorld()->SpawnActor<AActor>(wateringCanPreview, placementPoint, UKismetMathLibrary::MakeRotator(0, 0, 0), myParams);
			placementPreview->SetHidden(false);
		}
		else if(IsValid(RV_Hit.GetActor()) && RV_Hit.GetActor()->ActorHasTag("GrowthPlot") && toolStatus == PlayerToolStatus::ShovelOut)
		{
			placementPreview->SetActorLocation(RV_Hit.GetActor()->GetActorLocation());
			if (placementPreview->GetClass()->ImplementsInterface(UItemHighlightInterface::StaticClass()))
			{
				IItemHighlightInterface::Execute_ChangeHighlight(placementPreview, true);
			}
		}
		else if(IsValid(RV_Hit.GetActor()) && surfaceHit == SurfaceTypeGround && toolStatus == PlayerToolStatus::ShovelOut)
		{
			if (placementPreview->GetClass()->ImplementsInterface(UItemHighlightInterface::StaticClass()))
			{
				IItemHighlightInterface::Execute_ChangeHighlight(placementPreview, false);
			}

			placementPreview->SetActorLocation(placementPoint);
			placementPreview->SetActorHiddenInGame(false);
		}
		else if(toolStatus == PlayerToolStatus::WateringCanOut)
		{
			if (IsValid(RV_Hit.GetActor()) && RV_Hit.GetActor()->ActorHasTag("GrowthPlot"))
			{
				placementPoint.Z = RV_Hit.GetActor()->GetActorLocation().Z;
			}

			placementPreview->SetActorLocation(placementPoint);
			placementPreview->SetActorHiddenInGame(false);
		}
		else
		{
			placementPreview->SetActorHiddenInGame(true);
		}
	}
	else if (curPlayerStatus == PlayerStatus::Placement)
	{
		FHitResult RV_Hit;
		RV_Hit = placementLineTraceDown();

		FVector placementPoint = RV_Hit.ImpactPoint;

		if (curSelectedItemSlot >= 0 && !IsValid(placementPreview) && curPlayerStatus == PlayerStatus::Placement)
		{
			TArray<FName> rowNames = placeablesTable->GetRowNames();
			FName curItemID = myInventoryComp->getItemAtSlot(curSelectedItemSlot).item->uniqueID;
			FInvTableItem* itemToPlace = placeablesTable->FindRow<FInvTableItem>(curItemID, FString(""));

			if(itemToPlace == nullptr)
				return;

			FActorSpawnParameters myParams;
			myParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			placementPreview = GetWorld()->SpawnActor<AActor>(itemToPlace->itemBlueprint, placementPoint, UKismetMathLibrary::MakeRotator(0, 0, 0), myParams);
			placementPreview->SetActorEnableCollision(false);
			placementPreview->SetHidden(false);
		}
		if (IsValid(RV_Hit.GetActor()) && RV_Hit.GetActor()->ActorHasTag("Placeable"))
		{
			if (IsValid(placementPreview))
			{
				placementPreview->SetActorHiddenInGame(true);
			}

			if (RV_Hit.GetActor()->GetClass()->ImplementsInterface(UItemHighlightInterface::StaticClass()))
			{
				IItemHighlightInterface::Execute_ChangeHighlight(RV_Hit.GetActor(), true);
				lastHighlighted = RV_Hit.GetActor();
			}
		}
		else if(IsValid(placementPreview))
		{
			if (IsValid(lastHighlighted))
			{
				IItemHighlightInterface::Execute_ChangeHighlight(lastHighlighted, false);
				placementPreview->SetActorHiddenInGame(false);
				lastHighlighted = nullptr;
			}

			FRotator newRot = GetActorRotation();
			newRot.Yaw = (round(newRot.Yaw / 90) * 90);
			newRot.Pitch = 0;
			newRot.Roll = 0;

			if(UKismetMathLibrary::Abs(newRot.Yaw) == 180)
				newRot.Yaw = 0;
			else if(UKismetMathLibrary::Abs(newRot.Yaw) == 0)
				newRot.Yaw = 180;
			else
				newRot.Yaw *= -1;

			FVector previewOrigin;
			FVector previewBounds;

			placementPreview->GetActorBounds(false, previewOrigin, previewBounds, false);
			placementPoint.Z += previewBounds.Z;


			placementPreview->SetActorRotation(newRot);
			placementPreview->SetActorLocation(placementPoint);
			objectToTrack = placementPreview;
		}
		else if (IsValid(lastHighlighted))
		{
			if (IsValid(lastHighlighted) && lastHighlighted->GetClass()->ImplementsInterface(UItemHighlightInterface::StaticClass()))
			{
				IItemHighlightInterface::Execute_ChangeHighlight(lastHighlighted, false);
				lastHighlighted = nullptr;
				objectToTrack = nullptr;
			}
		}
	}
	else if (curFishingIndicator && curPlayerStatus == PlayerStatus::FishingCasting)
	{
		if (curDistance >= maxCastDistance)
		{
			curDistance = 1;
			bool waterHit = checkForPhysMat(curFishingIndicator->GetActorLocation(), SurfaceTypeWater);

			if (waterHit)
			{
				setPlayerStatus(PlayerStatus::Fishing);
				if (GetWorld())
				{
					FRotator newRot = GetActorRotation();
					newRot = UKismetMathLibrary::MakeRotator(0, 0, newRot.Yaw);

					FActorSpawnParameters myParams;
					myParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

					float qualityChance = UKismetMathLibrary::RandomFloatInRange(0, 1);

					curFishingMiniGame = GetWorld()->SpawnActor<AActor>(fishingMiniGameBlueprint, curFishingIndicator->GetActorLocation(), newRot);
					objectToTrack = curFishingMiniGame;

					if (qualityChance >= 0.95)
					{
						Cast<AFishingMiniGame>(curFishingMiniGame)->setDifficulty(FishingDifficulty::Hard);
					}
					else if (qualityChance >= 0.9)
					{
						Cast<AFishingMiniGame>(curFishingMiniGame)->setDifficulty(FishingDifficulty::Normal);
					}

					Cast<AFishingMiniGame>(curFishingMiniGame)->fishingFinished.AddDynamic(this, &AFarmSimCharacter::fishingMiniGameDelegateFunc);
					Cast<AFishingMiniGame>(curFishingMiniGame)->fishingFinished.AddDynamic(this, &AFarmSimCharacter::fishingMiniGameDelegateFunc);
				}
			}
			else
			{
				toolUsed = false;
				objectToTrack = nullptr;
				setPlayerStatus(PlayerStatus::NormalState);
			}

			curFishingIndicator->Destroy();
		}
		else
		{
			FHitResult findGround = placementLineTraceDown(false, false, curDistance, 2000);
			curFishingIndicator->SetActorLocation(findGround.ImpactPoint);

			curDistance += DeltaTime;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input
void AFarmSimCharacter::CameraUpAction_Implementation(float Rate)
{
	if (curPlayerStatus != PlayerStatus::NormalState && curPlayerStatus != PlayerStatus::Fishing && curPlayerStatus != PlayerStatus::FishingCasting && curPlayerStatus != PlayerStatus::Placement)
		return;

	// calculate delta for this frame from the rate information
	AddControllerPitchInput(TurnRate * Rate);// *TurnRate* GetWorld()->GetDeltaSeconds());
}

void AFarmSimCharacter::CameraRightAction_Implementation(float Rate)
{
	if (curPlayerStatus != PlayerStatus::NormalState && curPlayerStatus != PlayerStatus::Fishing && curPlayerStatus != PlayerStatus::FishingCasting && curPlayerStatus != PlayerStatus::Placement)
		return;

	// calculate delta for this frame from the rate information
	AddControllerYawInput(TurnRate * Rate);// *TurnRate* GetWorld()->GetDeltaSeconds());
}

void AFarmSimCharacter::MoveForwardAction_Implementation(float Value)
{
	if (!toolUsed && (Controller != nullptr) && (Value != 0.0f) && (curPlayerStatus == PlayerStatus::NormalState || curPlayerStatus == PlayerStatus::Placement))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AFarmSimCharacter::MoveRightAction_Implementation(float Value)
{
	if (!toolUsed && (Controller != nullptr) && (Value != 0.0f) && (curPlayerStatus == PlayerStatus::NormalState || curPlayerStatus == PlayerStatus::Placement))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AFarmSimCharacter::SprintAction_Implementation(float Value)
{
	if (!toolUsed && (Controller != nullptr) && (curPlayerStatus == PlayerStatus::NormalState || curPlayerStatus == PlayerStatus::Placement))
	{
		UCharacterMovementComponent* myMoveComp = Cast<UCharacterMovementComponent>(GetComponentByClass(UCharacterMovementComponent::StaticClass()));
		if (myMoveComp->MaxWalkSpeed == walkSpeed)
		{
			return;
		}
		else if (Value == 1)
		{
			myMoveComp->MaxWalkSpeed = runSpeed;
		}
		else
		{
			myMoveComp->MaxWalkSpeed = normalSpeed;
		}
	}
}

void AFarmSimCharacter::WalkAction_Implementation(float Value)
{
	if (!toolUsed && (Controller != nullptr) && (curPlayerStatus == PlayerStatus::NormalState || curPlayerStatus == PlayerStatus::Placement))
	{
		UCharacterMovementComponent* myMoveComp = Cast<UCharacterMovementComponent>(GetComponentByClass(UCharacterMovementComponent::StaticClass()));

		if (myMoveComp->MaxWalkSpeed == runSpeed)
		{
			return;
		}
		else if (Value == 1)
		{
			myMoveComp->MaxWalkSpeed = walkSpeed;
		}
		else
		{
			myMoveComp->MaxWalkSpeed = normalSpeed;
		}
	}
}

void AFarmSimCharacter::InteractAction_Implementation()
{
	if (curPlayerStatus == PlayerStatus::InMenu)
	{
		if (IsValid(interactActorComp) && (interactActorComp->interactionType == "Loot" || interactActorComp->interactionType == "Crafting") && interactActorComp->GetClass()->ImplementsInterface(UInteractInterface::StaticClass()))
		{
			IInteractInterface::Execute_Interact(interactActorComp);

			if (IsValid(lastHighlighted) && lastHighlighted->GetClass()->ImplementsInterface(UItemHighlightInterface::StaticClass()))
			{
				IItemHighlightInterface::Execute_ChangeHighlight(lastHighlighted, false);
				lastHighlighted = nullptr;
			}
		}

		EscMenuAction();
		return;
	}

	if (curPlayerStatus == PlayerStatus::Fishing)
	{
		if (curFishingMiniGame->GetClass()->ImplementsInterface(UInteractInterface::StaticClass()))
		{
			IInteractInterface::Execute_Interact(curFishingMiniGame);
		}
	}
	else if (curPlayerStatus == PlayerStatus::Placement)
	{
		if (curSelectedItemSlot >= 0)
		{
			placePlaceable();
		}
		else
		{
			displayNotification("Not items to place");
		}
	}
	else if (interactActorComp != nullptr)//If we are interacting with something using an interaction component
	{
		if (curPlayerStatus == PlayerStatus::InMenu && interactActorComp->interactionType == "Loot")
		{
			setPlayerStatus(PlayerStatus::NormalState);
			toggleMenuUI(false, "Inventory", false);
		}
		else if (interactActorComp->interactionType == "Loot")
		{
			setPlayerStatus(PlayerStatus::InMenu);
			toggleMenuUI(true, "Inventory", false);
		}
		else if (curPlayerStatus == PlayerStatus::Planting)
		{
			FName cropToPlant = myInventoryComp->getItemAtSlot(curSelectedItemSlot).item->uniqueID;
			AGrowthPlot* curPlot = Cast<AGrowthPlot>(interactActorComp->GetOwner());
			bool tryPlant = curPlot->plantCrop(cropToPlant);
			
			if (tryPlant)
			{
				if (curPlot->GetClass()->ImplementsInterface(UItemHighlightInterface::StaticClass()))
				{
					IItemHighlightInterface::Execute_ChangeHighlight(curPlot, false);
					lastHighlighted = nullptr;
				}

				myInventoryComp->changeQuantity(cropToPlant, -1);

				setPlayerStatus(PlayerStatus::NormalState);
				setSelectedItem(false);
			}
			else
			{
				displayNotification("This plant cannot be planted during this season.");
			}

			return;
		}
		else if (interactActorComp->interactionType == "GrowthPlot")
		{
			if (curSelectedItemSlot == -2)
			{
				curSelectedItemSlot = myInventoryComp->findNextItemOfType(curSelectedItemSlot, 1, "Seed");
			}

			if (curSelectedItemSlot != -1)
			{
				setSelectedItem();
				setPlayerStatus(PlayerStatus::Planting);
			}
			else
			{
				displayNotification("No seeds in your inventory to plant");
				curSelectedItemSlot = -2;
			}
		}

		if (interactActorComp->GetClass()->ImplementsInterface(UInteractInterface::StaticClass()))
		{
			IInteractInterface::Execute_Interact(interactActorComp);
		}
	}
	else//If no component use tool
	{
		UseToolAction();
	}
}

void AFarmSimCharacter::UseToolAction_Implementation()
{
	if (curPlayerStatus == PlayerStatus::Placement)
	{
		placePlaceable(false);
		return;
	}
	else if (toolUsed && toolStatus != PlayerToolStatus::FishingRodOut)
	{
		return;
	}
	else if (curPlayerStatus == PlayerStatus::InMenu)
	{
		return;
	}

	switch (toolStatus)
	{
	case PlayerToolStatus::ShovelOut: //Line trace down digDistance in front of the player and check if the ground is free if so create a growth plot there
	{
		FHitResult RV_Hit = placementLineTraceDown();

		EPhysicalSurface surfaceHit = UGameplayStatics::GetSurfaceType(RV_Hit);

		if (IsValid(RV_Hit.GetActor()) && RV_Hit.GetActor()->ActorHasTag("GrowthPlot"))
		{
			if (!Cast<AGrowthPlot>(RV_Hit.GetActor())->isInUse())
			{
				toolUsed = true;
				RV_Hit.GetActor()->Destroy();
			}
			else
			{
				return;
			}
		}
		else if (surfaceHit == SurfaceTypeGround)
		{
			//Currenlty only checking the middle, check if the slope is bigger than the maxiumun slope for placeable objects
			if (UKismetMathLibrary::Abs(RV_Hit.Normal.Y) > maxSlope)
			{
				displayNotification("Ground is not flat enough");
				return;
			}
			else if (curPlayerLocation != LocationStatus::HomeYard)
			{
				displayNotification("You may not dig in this location.");
				return;
			}
			else
			{
				toolUsed = true;
				FVector placementPoint = RV_Hit.ImpactPoint;
				placementPoint.Z += growthPlotZExtent;

				FActorSpawnParameters myParams;
				myParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
				AActor* newPlot = GetWorld()->SpawnActor<AActor>(growthPlotActor, placementPoint, UKismetMathLibrary::MakeRotator(0, 0, 0), myParams);
			}
		}
		break;
	}
	case PlayerToolStatus::WateringCanOut: //Box Trace down digDistance in front of player and try to water all plants hit
	{
		toolUsed = true;
		TArray<FHitResult> RV_Hits;
		TArray<AActor*> actorsToIgnore;
		actorsToIgnore.Add(this);

		FVector startPoint = GetActorLocation() + (GetActorForwardVector() * digDistance);
		FVector endPoint = startPoint;
		endPoint.Z -= 300;

		UKismetSystemLibrary::BoxTraceMulti(
			this, 
			startPoint, 
			endPoint,
			FVector(150,150,20),
			FRotator(0,0,0),
			ETraceTypeQuery::TraceTypeQuery3,
			true,
			actorsToIgnore,
			EDrawDebugTrace::None,
			RV_Hits,
			true);

		for (const FHitResult hit : RV_Hits)
		{
			if (IsValid(hit.GetActor()))
			{
				ABasePlant* curPlant = Cast<ABasePlant>(hit.GetActor());

				if (IsValid(curPlant))
				{
					curPlant->waterPlant();
				}
			}
		}
		break;
	}
	case PlayerToolStatus::PickaxeOut://These two just send a message to interact with the harvest locations so that interact and use tool can be done using either key
	case PlayerToolStatus::AxeOut:
		if (IsValid(interactActorComp))
		{
			if (interactActorComp->GetClass()->ImplementsInterface(UInteractInterface::StaticClass()))
			{
				IInteractInterface::Execute_Interact(interactActorComp);
			}
		}
		break;
	case PlayerToolStatus::FishingRodOut:
		if (curPlayerStatus == PlayerStatus::Fishing)
		{
			InteractAction();
		}
		break;
	default:
		break;
	}
}


//check if it was in water at the end then start the minigame if it was
//minigame like disney for now, circle closes in press interact/use tool while in the circle
//if successful pull random fish from this areas fish table and add it to inventory if room or drop it on the ground if no room

//Separate function used for fishing casting because I need to use pressed and released actions
void AFarmSimCharacter::FishingCastAction_Implementation(bool pressed)
{
	if(curPlayerStatus == PlayerStatus::Fishing || IsValid(curFishingMiniGame))
		return;

	if (pressed && curPlayerStatus == PlayerStatus::NormalState)
	{
		toolUsed = true;
		setPlayerStatus(PlayerStatus::FishingCasting);

		FActorSpawnParameters myParams;
		myParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FHitResult findGround = placementLineTraceDown(false, false);

		curFishingIndicator = GetWorld()->SpawnActor<AActor>(fishingIndicatorBlueprint, findGround.ImpactPoint, GetActorRotation(), myParams);
		objectToTrack = curFishingIndicator;
	}
	else if(curPlayerStatus == PlayerStatus::FishingCasting)
	{
		curDistance = 1;
		bool waterHit = checkForPhysMat(curFishingIndicator->GetActorLocation(), SurfaceTypeWater);

		if (waterHit)
		{
			setPlayerStatus(PlayerStatus::Fishing);
			if (GetWorld())
			{
				FRotator newRot = GetActorRotation();
				newRot = UKismetMathLibrary::MakeRotator(0, 0, newRot.Yaw);

				FActorSpawnParameters myParams;
				myParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				float qualityChance = UKismetMathLibrary::RandomFloatInRange(0, 1);

				curFishingMiniGame = GetWorld()->SpawnActor<AActor>(fishingMiniGameBlueprint, curFishingIndicator->GetActorLocation(), newRot);
				objectToTrack = curFishingMiniGame;

				if (qualityChance >= 0.95)
				{
					Cast<AFishingMiniGame>(curFishingMiniGame)->setDifficulty(FishingDifficulty::Hard);
				}
				else if (qualityChance >= 0.9)
				{
					Cast<AFishingMiniGame>(curFishingMiniGame)->setDifficulty(FishingDifficulty::Normal);
				}

				Cast<AFishingMiniGame>(curFishingMiniGame)->fishingFinished.AddDynamic(this, &AFarmSimCharacter::fishingMiniGameDelegateFunc);
				Cast<AFishingMiniGame>(curFishingMiniGame)->newFishingRecord.AddDynamic(this, &AFarmSimCharacter::fishingRecordDelegateFunc);
			}
		}
		else
		{
			toolUsed = false;
			objectToTrack = nullptr;
			setPlayerStatus(PlayerStatus::NormalState);
		}

		curFishingIndicator->Destroy();
	}
}



void AFarmSimCharacter::JumpAction_Implementation(bool isJumping)
{
	if(curPlayerStatus != PlayerStatus::NormalState)
		return;

	if (isJumping)
	{
		Jump();
	}
	else
	{
		StopJumping();
	}
}

//Scrolling through various things
void AFarmSimCharacter::ScrollItemsAction_Implementation(float Value)
{
	if (curPlayerStatus == PlayerStatus::NormalState && otherInteractComps.Num() > 1)//Scroll through overlapped interactable objects
	{
		int newIndex = otherInteractComps.Find(interactActorComp);

		//Find new index
		if (Value > 0)
		{
			++newIndex;

			if (newIndex >= otherInteractComps.Num())
			{
				newIndex = 0;
			}
		}
		else if(Value < 0)
		{
			--newIndex;

			if (newIndex < 0)
			{
				newIndex = otherInteractComps.Num() - 1;
			}
		}

		//Disable old highlight
		if (IsValid(lastHighlighted) && lastHighlighted->GetClass()->ImplementsInterface(UItemHighlightInterface::StaticClass()))
		{
			IItemHighlightInterface::Execute_ChangeHighlight(lastHighlighted, false);
		}

		//Change selected, update ui, and enable new highlight
		interactActorComp = otherInteractComps[newIndex];
		objectToTrack = interactActorComp->GetOwner();
		updateInteractPrompt();

		if (interactActorComp->GetOwner()->GetClass()->ImplementsInterface(UItemHighlightInterface::StaticClass()))
		{
			lastHighlighted = interactActorComp->GetOwner();
			IItemHighlightInterface::Execute_ChangeHighlight(lastHighlighted, true);
		}

	}
	else if (!toolUsed && curPlayerStatus == PlayerStatus::NormalState && Value != 0)//Scroll through equipped tools
	{
		uint8 curTool = (uint8)toolStatus;
		if(Value > 0)
		{
			++curTool;
		}
		else if(Value < 0)
		{
			--curTool;
		}

		if (curTool <= 0)
		{
			curTool = 5;
		}
		else if (curTool > 5)
		{
			curTool = 1;
		}

		changeEquippedTool((PlayerToolStatus)curTool);
	}
	else if(curPlayerStatus == PlayerStatus::Planting)//Scroll through items for planting or placement
	{
		FName itemType;

		switch (curPlayerStatus)
		{	
		case PlayerStatus::Planting:
			itemType = "Seed";
			break;
		case PlayerStatus::Placement:
			if (IsValid(placementPreview))
			{
				placementPreview->Destroy();
			}
			itemType = "Placeable";
			break;
		default:
			return;
			break;
		}
		
		int newSlot = -5;

		if (Value > 0)
		{
			newSlot = myInventoryComp->findNextItemOfType(curSelectedItemSlot, 1, itemType);
		}
		else if (Value < 0)
		{
			newSlot = myInventoryComp->findNextItemOfType(curSelectedItemSlot, -1, itemType);
		}

		if (newSlot == -1 && !myInventoryComp->itemTypeExists(itemType))
		{
			EscMenuAction();
		}
		else if(newSlot != -1 && myInventoryComp->itemTypeExists(itemType))
		{
			curSelectedItemSlot = newSlot;
			setSelectedItem();
		}
	}
}

void AFarmSimCharacter::changeEquippedTool_Implementation(PlayerToolStatus newTool)
{
	if (toolUsed || curPlayerStatus == PlayerStatus::InMenu)
	{
		return;
	}
	if (newTool == toolStatus)
	{
		prevToolStatus = toolStatus;
		toolStatus = PlayerToolStatus::NoToolOut;
		toolMesh->SetVisibility(false);
	}
	else
	{
		toolMesh->SetVisibility(true);
		toolMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		FName newSocket;
		switch (newTool)
		{

		case PlayerToolStatus::PickaxeOut:
			prevToolStatus = toolStatus;
			toolStatus = PlayerToolStatus::PickaxeOut;
			toolMesh->SetStaticMesh(currentTools["Pickaxe"]->model);
			newSocket = "pickSocket";
			break;
		case PlayerToolStatus::AxeOut:
			prevToolStatus = toolStatus;
			toolStatus = PlayerToolStatus::AxeOut;
			toolMesh->SetStaticMesh(currentTools["Axe"]->model);
			newSocket = "axeSocket";
			break;
		case PlayerToolStatus::ShovelOut:
			prevToolStatus = toolStatus;
			toolStatus = PlayerToolStatus::ShovelOut;
			toolMesh->SetStaticMesh(currentTools["Shovel"]->model);
			newSocket = "shovelSocket";
			break;
		case PlayerToolStatus::WateringCanOut:
			prevToolStatus = toolStatus;
			toolStatus = PlayerToolStatus::WateringCanOut;
			toolMesh->SetStaticMesh(currentTools["WaterCan"]->model);
			newSocket = "watercanSocket";
			break;
		case PlayerToolStatus::FishingRodOut:
			prevToolStatus = toolStatus;
			toolStatus = PlayerToolStatus::FishingRodOut;
			toolMesh->SetStaticMesh(currentTools["FishingRod"]->model);
			newSocket = "rodSocket";
			break;
		default:
			break;
		}
		
		reAttachTool(newSocket);
	}

	//Broadcast change for UI and model changes
	PlayerToolChanged.Broadcast(toolStatus);

	if ((prevToolStatus == PlayerToolStatus::ShovelOut || prevToolStatus == PlayerToolStatus::WateringCanOut) && IsValid(placementPreview))
	{
		placementPreview->Destroy();
		placementPreview = nullptr;
		objectToTrack = nullptr;
	}
}

//Enter/Exit Placementmode
void AFarmSimCharacter::togglePlacementModeAction_Implementation()
{
	if (curPlayerStatus == PlayerStatus::Placement)
	{
		curSelectedItemSlot = -2;
		setSelectedItem(false);
		setPlayerStatus(PlayerStatus::NormalState);

		if (IsValid(placementPreview))
		{
			placementPreview->Destroy();
		}
	}
	else if(curPlayerStatus == PlayerStatus::NormalState)
	{
		changeEquippedTool(toolStatus);
		curSelectedItemSlot = myInventoryComp->findNextItemOfType(-2, 1, "Placeable");
		setPlayerStatus(PlayerStatus::Placement);
		setSelectedItem();
	}
}

//Close the game or back out of menus
void AFarmSimCharacter::EscMenuAction_Implementation()
{
	switch (curPlayerStatus)
	{
	case PlayerStatus::Planting:
		setPlayerStatus(PlayerStatus::NormalState);
		setSelectedItem(false);
		curSelectedItemSlot = -2;
		if(IsValid(placementPreview))
			placementPreview->Destroy();
		break;
	case PlayerStatus::Placement:
		setPlayerStatus(PlayerStatus::NormalState);
		setSelectedItem(false);
		curSelectedItemSlot = -2;
		if (IsValid(placementPreview))
			placementPreview->Destroy();
		break;
	case PlayerStatus::NormalState:
		toggleMenuUI(true, "Settings");
		break;
	case PlayerStatus::InMenu:
		toggleMenuUI(false);
		break;
	default:
		break;
	}
}


//Change current player status and broadcast to all those listening
void AFarmSimCharacter::setPlayerStatus(PlayerStatus newStatus)
{
	prevPlayerStatus = curPlayerStatus;
	curPlayerStatus = newStatus;
	PlayerStatusChanged.Broadcast(curPlayerStatus);

	if (prevPlayerStatus == PlayerStatus::Placement && IsValid(lastHighlighted))
	{
		IItemHighlightInterface::Execute_ChangeHighlight(lastHighlighted, false);
		lastHighlighted = nullptr;
	}
}

void AFarmSimCharacter::changeTool(const FName toolType, UToolItemAsset* newTool)
{
	if (currentTools.Contains(toolType))
	{
		currentTools[toolType] = newTool;
	}
}

UToolItemAsset* AFarmSimCharacter::grabTool(const FName toolType)
{
	if (currentTools.Contains(toolType))
	{
		return currentTools[toolType];
	}
	else
	{
		return currentTools["Pickaxe"];//shouldnt be possible to hit this
	}
}


//Reuseable line trace down digDistance in front of player and snapped to grid
FHitResult AFarmSimCharacter::placementLineTraceDown(bool snapToGrid, bool drawDebug, float distanceMod, float distanceDown)
{
	FHitResult RV_Hit(ForceInit);
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), false, this);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;
	RV_TraceParams.AddIgnoredActor(this);
	RV_TraceParams.bReturnPhysicalMaterial = true;
	
	if (IsValid(placementPreview))
	{
		RV_TraceParams.AddIgnoredActor(placementPreview);
	}

	if (drawDebug)
	{
		const FName TraceTag("MyTraceTag");
		//GetWorld()->DebugDrawTraceTag = TraceTag;
		RV_TraceParams.TraceTag = TraceTag;
	}

	FVector startPoint = GetActorLocation() + (GetActorForwardVector() * digDistance * distanceMod);

	if (snapToGrid)
	{
		startPoint.X = UKismetMathLibrary::GridSnap_Float(startPoint.X, gridSnap);
		startPoint.Y = UKismetMathLibrary::GridSnap_Float(startPoint.Y, gridSnap);
	}

	FVector endPoint = startPoint;
	endPoint.Z -= distanceDown;


	bool hit = GetWorld()->LineTraceSingleByChannel(
		RV_Hit,
		startPoint,
		endPoint,
		ECC_Visibility,
		RV_TraceParams
	);

	return RV_Hit;
}

void AFarmSimCharacter::fishingMiniGameDelegateFunc(bool Status, const FInvItem& FishCaught)
{
	if (!Status)
	{
		displayNotification("The fish got away.");
	}
	else
	{
		caughtFish.Broadcast(Cast<UFishItemAsset>(FishCaught.item));
		FString notif = "You caught " + FString::FromInt(FishCaught.quantity) + " " + FishCaught.item->name.ToString() + ".";
		displayNotification(notif);

		FAddItemStatus addFish = myInventoryComp->addNewItem(FishCaught, true);
	}

	toolUsed = false;
	objectToTrack = nullptr;
	setPlayerStatus(PlayerStatus::NormalState);
}

void AFarmSimCharacter::fishingRecordDelegateFunc(FFishRecordStruct fishRecord)
{
	switch (fishRecord.record)
	{
	case NoChange:
		break;
	case NewMin:
	case NewMax:
	case NewFish:
		newFishingRecordChange.Broadcast(fishRecord);
		break;
	default:
		break;
	}
}

bool AFarmSimCharacter::checkForPhysMat(FVector location, EPhysicalSurface surfaceToCheckFor)
{
	FHitResult RV_Hit(ForceInit);
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), false, this);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.AddIgnoredActor(this);
	RV_TraceParams.bReturnPhysicalMaterial = true;

	FVector startPoint = location;
	startPoint.Z += 20;
	FVector endPoint = location;
	endPoint.Z -= 100;

	bool hit = GetWorld()->LineTraceSingleByChannel(
		RV_Hit,
		startPoint,
		endPoint,
		ECC_Visibility,
		RV_TraceParams
	);

	EPhysicalSurface surfaceHit = UGameplayStatics::GetSurfaceType(RV_Hit);

	if (surfaceHit != NULL && surfaceHit == surfaceToCheckFor)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void AFarmSimCharacter::displayNotification(const FString& message, int showLength)
{
	if (message.Len() <= 0 || showLength <= 0 || message == curNotification)
	{
		return;
	}
	else
	{
		TTuple<FString, int> newNotif;
		newNotif.Key = message;
		newNotif.Value = showLength;
		notificationQueue.Enqueue(newNotif);
	}
}


//Place or remove placeable in front of player
void AFarmSimCharacter::placePlaceable(bool place)
{
	FHitResult RV_Hit = placementLineTraceDown();

	EPhysicalSurface surfaceHit = UGameplayStatics::GetSurfaceType(RV_Hit);

	if (place && surfaceHit == SurfaceTypeGround)
	{
		//Currenlty only checking the middle, check if the slope is bigger than the maxiumun slope for placeable objects
		if (UKismetMathLibrary::Abs(RV_Hit.Normal.Y) > maxSlope)
		{
			displayNotification("Ground is not flat enough");
			return;
		}
		else
		{
			TArray<FName> rowNames = placeablesTable->GetRowNames();
			FName curItemID = myInventoryComp->getItemAtSlot(curSelectedItemSlot).item->uniqueID;
			FInvTableItem* itemToPlace = placeablesTable->FindRow<FInvTableItem>(curItemID, FString(""));

			if (!itemToPlace->placeableBiome.Contains(curPlayerLocation))
			{
				displayNotification("You cannot place that in this zone");
				return;
			}
			else
			{
				FRotator newRot = GetActorRotation();
				newRot.Pitch = 0;
				newRot.Roll = 0;
				newRot.Yaw = (round(newRot.Yaw / 90) * 90);

				if (UKismetMathLibrary::Abs(newRot.Yaw) == 180)
					newRot.Yaw = 0;
				else if (UKismetMathLibrary::Abs(newRot.Yaw) == 0)
					newRot.Yaw = 180;
				else
					newRot.Yaw *= -1;

				FActorSpawnParameters myParams;
				myParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
				AActor* newPlaceable = GetWorld()->SpawnActor<AActor>(itemToPlace->itemBlueprint, FVector(0,0,0), newRot, myParams);
				newPlaceable->SetActorTickEnabled(true);
				if (!IsValid(newPlaceable))
				{
					displayNotification("Area blocked");
					return;

				}


				FVector previewOrigin;
				FVector previewBounds;
				placementPreview->GetActorBounds(false, previewOrigin, previewBounds, false);

				FVector placementPoint = RV_Hit.ImpactPoint;
				placementPoint.Z += previewBounds.Z;

				newPlaceable->SetActorLocation(placementPoint);
		

				myInventoryComp->changeQuantity(curItemID, -1);
				int amountCheck = myInventoryComp->getItemQuantity(curItemID);
				if (amountCheck <= 0)
				{
					ScrollItemsAction(1);
				}
				else
				{
					setSelectedItem();
				}
			}
		}
	}
	else if (!place && IsValid(RV_Hit.GetActor()) && RV_Hit.GetActor()->ActorHasTag("Placeable"))
	{
		TArray<FName> rowNames = placeablesTable->GetRowNames();
		for (int i = 0; i < rowNames.Num(); ++i)
		{
			FInvTableItem* curRow = placeablesTable->FindRow<FInvTableItem>(rowNames[i], FString(""));
			if (curRow->itemBlueprint == RV_Hit.GetActor()->GetClass())
			{
				UInventoryComponent* itemsInv = Cast<UInventoryComponent>(RV_Hit.GetActor()->GetComponentByClass(UInventoryComponent::StaticClass()));
				if (itemsInv != nullptr && !itemsInv->isEmpty())
				{
					displayNotification("You cannot pick up a storage device with items in it.");
					return;
				}
				else
				{
					FInvItem pickup = FInvItem();
					pickup.item = Cast<UItemAsset>(curRow->item);
					pickup.quantity = 1;

					myInventoryComp->addNewItem(pickup, true, true);

					RV_Hit.GetActor()->Destroy();
				}
				break;
			}
		}
	}
}

void AFarmSimCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer = playerTags;
}