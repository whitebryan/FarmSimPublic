// Copyright Epic Games, Inc. All Rights Reserved.

#include "FarmSimCharacter.h"
#include "SimpleInteract/Public/InteractInterface.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GrowthPlot.h"
#include "BasePlant.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

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
}

void AFarmSimCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (toolStatus == PlayerToolStatus::ShovelOut || toolStatus == PlayerToolStatus::WateringCanOut)//Create and show a placement preview for digging a growth plot or watering crops
	{
		FHitResult RV_Hit;
		RV_Hit = placementLineTraceDown();

		FVector placementPoint = RV_Hit.ImpactPoint;
		placementPoint.Z += growthPlotZExtent;

		if (!IsValid(placementPreview) && toolStatus == PlayerToolStatus::ShovelOut)
		{
			FActorSpawnParameters myParams;
			myParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			placementPreview = GetWorld()->SpawnActor<AActor>(growthPlotPreview, placementPoint, UKismetMathLibrary::MakeRotator(0, 0, 0), myParams);
			placementPreview->SetHidden(false);
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
			placementPreview->SetActorHiddenInGame(false);
		}
		else if(IsValid(RV_Hit.GetActor()) && RV_Hit.GetActor()->ActorHasTag("Ground") && toolStatus == PlayerToolStatus::ShovelOut)
		{
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
}

//////////////////////////////////////////////////////////////////////////
// Input
void AFarmSimCharacter::CameraUpAction_Implementation(float Rate)
{
	if (curPlayerStatus != PlayerStatus::NormalState)
		return;

	// calculate delta for this frame from the rate information
	AddControllerPitchInput(TurnRate * Rate);// *TurnRate* GetWorld()->GetDeltaSeconds());
}

void AFarmSimCharacter::CameraRightAction_Implementation(float Rate)
{
	if (curPlayerStatus != PlayerStatus::NormalState)
		return;

	// calculate delta for this frame from the rate information
	AddControllerYawInput(TurnRate * Rate);// *TurnRate* GetWorld()->GetDeltaSeconds());
}

void AFarmSimCharacter::MoveForwardAction_Implementation(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f) && curPlayerStatus == PlayerStatus::NormalState)
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
	if ( (Controller != nullptr) && (Value != 0.0f) && curPlayerStatus == PlayerStatus::NormalState)
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

void AFarmSimCharacter::InteractAction_Implementation()
{
	if (interactActorComp != nullptr)//If we are interacting with something using an interaction component
	{
		if (curPlayerStatus == PlayerStatus::InStorage && interactActorComp->interactionType == "Loot")
		{
			setPlayerStatus(PlayerStatus::NormalState);
		}
		else if (interactActorComp->interactionType == "Loot")
		{
			setPlayerStatus(PlayerStatus::InStorage);
		}
		else if (curPlayerStatus == PlayerStatus::Planting)
		{
			FName cropToPlant = myInventoryComp->getItemAtSlot(curSelectedSeedSlot).name;
			AGrowthPlot* curPlot = Cast<AGrowthPlot>(interactActorComp->GetOwner());
			curPlot->plantCrop(cropToPlant);
			int test = myInventoryComp->changeQuantity(cropToPlant, -1);

			setPlayerStatus(PlayerStatus::NormalState);
			setSelectedItem(false);
			return;
		}
		else if (interactActorComp->interactionType == "GrowthPlot")
		{
			if (curSelectedSeedSlot == -2)
			{
				curSelectedSeedSlot = myInventoryComp->findNextItemOfType(curSelectedSeedSlot, 1, "Seed");
			}

			if (curSelectedSeedSlot != -1)
			{
				setSelectedItem();
				setPlayerStatus(PlayerStatus::Planting);
			}
			else
			{
				curSelectedSeedSlot = -2;
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
	switch (toolStatus)
	{
	case PlayerToolStatus::FishingRodOut:
	{
		break;
	}
	case PlayerToolStatus::ShovelOut: //Line trace down digDistance in front of the player and check if the ground is free if so create a growth plot there
	{
		FHitResult RV_Hit = placementLineTraceDown();

		if (IsValid(RV_Hit.GetActor()) && RV_Hit.GetActor()->ActorHasTag("GrowthPlot"))
		{
			if (!Cast<AGrowthPlot>(RV_Hit.GetActor())->isInUse())
			{
				RV_Hit.GetActor()->Destroy();
			}
			else
			{
				return;
			}
		}
		else if (IsValid(RV_Hit.GetActor()) && RV_Hit.GetActor()->ActorHasTag("Ground"))//Revist this later for better way to determine if its hitting the ground or not
		{
			FString actName = RV_Hit.GetActor()->GetName();
			//Currenlty only checking the middle, check if the slope is bigger than the maxiumun slope for placeable objects
			if (UKismetMathLibrary::Abs(RV_Hit.Normal.Y) > maxSlope)
			{
				return;
			}
			else
			{
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
		InteractAction();
		break;
	case PlayerToolStatus::AxeOut:
		InteractAction();
		break;
	default:
		break;
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

//Scrolling through inventory items
void AFarmSimCharacter::ScrollItemsAction_Implementation(float Value)
{
	if (curPlayerStatus == PlayerStatus::Planting)//Scroll through seeds for planting
	{
		if (Value > 0)
		{
			curSelectedSeedSlot = myInventoryComp->findNextItemOfType(curSelectedSeedSlot, 1, "Seed");
			setSelectedItem();
		}
		else if(Value < 0)
		{
			curSelectedSeedSlot = myInventoryComp->findNextItemOfType(curSelectedSeedSlot, -1, "Seed");
			setSelectedItem();
		}
	}
	else if (curPlayerStatus == PlayerStatus::NormalState && Value != 0)//Scroll through equipped tools
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
}

void AFarmSimCharacter::changeEquippedTool_Implementation(PlayerToolStatus newTool)
{
	if (newTool == toolStatus)
	{
		prevToolStatus = toolStatus;
		toolStatus = PlayerToolStatus::NoToolOut;
	}
	else
	{
		switch (newTool)
		{

		case PlayerToolStatus::PickaxeOut:
			prevToolStatus = toolStatus;
			toolStatus = PlayerToolStatus::PickaxeOut;
			break;
		case PlayerToolStatus::AxeOut:
			prevToolStatus = toolStatus;
			toolStatus = PlayerToolStatus::AxeOut;
			break;
		case PlayerToolStatus::ShovelOut:
			prevToolStatus = toolStatus;
			toolStatus = PlayerToolStatus::ShovelOut;
			break;
		case PlayerToolStatus::WateringCanOut:
			prevToolStatus = toolStatus;
			toolStatus = PlayerToolStatus::WateringCanOut;
			break;
		case PlayerToolStatus::FishingRodOut:
			prevToolStatus = toolStatus;
			toolStatus = PlayerToolStatus::FishingRodOut;
			break;
		default:
			break;
		}
	}

	//Broadcast change for UI and model changes
	PlayerToolChanged.Broadcast(toolStatus);

	if ((prevToolStatus == PlayerToolStatus::ShovelOut || prevToolStatus == PlayerToolStatus::WateringCanOut) && IsValid(placementPreview))
	{
		placementPreview->Destroy();
		placementPreview = nullptr;
	}
}

//Close the game or back out of menus
void AFarmSimCharacter::EscMenuAction_Implementation()
{
	switch (curPlayerStatus)
	{
	case PlayerStatus::InStorage:
		InteractAction();
		break;
	case PlayerStatus::Planting:
		setPlayerStatus(PlayerStatus::NormalState);
		setSelectedItem(false);
		break;
	case PlayerStatus::NormalState:
		//later open esc menu for now just close
		UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false);
		break;
	default:
		break;
	}
}

void AFarmSimCharacter::OpenInventoryAction_Implementation()
{
	//Nothing for now
}


//Change current player status and broadcast to all those listening
void AFarmSimCharacter::setPlayerStatus(PlayerStatus newStatus)
{
	prevPlayerStatus = curPlayerStatus;
	curPlayerStatus = newStatus;
	PlayerStatusChanged.Broadcast(curPlayerStatus);
}

void AFarmSimCharacter::changeTool(FName toolType, FToolInvItem newTool)
{
	for (int i = 0; i < currentTools.Num(); ++i)
	{
		if (currentTools[i].type == toolType)
		{
			if ((int)currentTools[i].toolTier < (int)newTool.toolTier)
			{
				currentTools[i] = newTool;
			}
		}
	}
}

FToolInvItem AFarmSimCharacter::grabTool(FName toolType)
{
	for (int i = 0; i < currentTools.Num(); ++i)
	{
		if (currentTools[i].type == toolType)
		{
			return currentTools[i];
		}
	}

	return currentTools[0];//Impossible to ever reach
}


//Reuseable line trace down digDistance in front of player and snapped to grid
FHitResult AFarmSimCharacter::placementLineTraceDown(bool snapToGrid, bool drawDebug)
{
	FHitResult RV_Hit(ForceInit);
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), false, this);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;
	RV_TraceParams.AddIgnoredActor(this);
	
	if (IsValid(placementPreview))
	{
		RV_TraceParams.AddIgnoredActor(placementPreview);
	}

	if (drawDebug)
	{
		const FName TraceTag("MyTraceTag");
		GetWorld()->DebugDrawTraceTag = TraceTag;
		RV_TraceParams.TraceTag = TraceTag;
	}

	FVector startPoint = GetActorLocation() + (GetActorForwardVector() * digDistance);

	if (snapToGrid)
	{
		startPoint.X = UKismetMathLibrary::GridSnap_Float(startPoint.X, gridSnap);
		startPoint.Y = UKismetMathLibrary::GridSnap_Float(startPoint.Y, gridSnap);
	}

	FVector endPoint = startPoint;
	endPoint.Z -= 300;


	bool hit = GetWorld()->LineTraceSingleByChannel(
		RV_Hit,
		startPoint,
		endPoint,
		ECC_Visibility,
		RV_TraceParams
	);

	return RV_Hit;
}
