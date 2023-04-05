// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseNPCController.h"
#include "GameFramework/Character.h" 
#include "Kismet/KismetMathLibrary.h" 
#include "Kismet/KismetStringLibrary.h" 
#include "BaseAICharacter.h"

ABaseNPCController::ABaseNPCController()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);
}

void ABaseNPCController::BeginPlay()
{
	Super::BeginPlay();

	//Finding player and possesed pawn
	playerCharacter = Cast<AActor>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	myCharacter = Cast<ABaseAICharacter>(AAIController::GetPawn());

	//Finding headd tracking capsule and interaction capsule
	TArray<UActorComponent*> myPawnsCapsules;
	AAIController::GetPawn()->GetComponents(myPawnsCapsules);
	USphereComponent* headTrackingCapsule = nullptr;
	mySphereComponent = nullptr;
	for (UActorComponent* curCapsule : myPawnsCapsules)
	{
		if (curCapsule->ComponentHasTag("headTracking"))
		{
			headTrackingCapsule = Cast<USphereComponent>(curCapsule);
		}
		else if (curCapsule->ComponentHasTag("interactionCapsule"))
		{
			mySphereComponent = Cast<USphereComponent>(curCapsule);
		}
	}

	if (IsValid(headTrackingCapsule))
	{
		//Bind to headd tracking capsule to toggle it when player is near or far
		headTrackingCapsule->OnComponentBeginOverlap.AddDynamic(this, &ABaseNPCController::OnOverlapBegin);
		headTrackingCapsule->OnComponentEndOverlap.AddDynamic(this, &ABaseNPCController::OnOverlapEnd);
	}
}


void ABaseNPCController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Checking if the player is infront of the pawn to trigger head tracking
	if (bShouldCheckForHeadTracking)
	{
		
		FVector headingVec = UKismetMathLibrary::GetDirectionUnitVector(myCharacter->GetActorLocation(), playerCharacter->GetActorLocation());
		headingVec.Normalize();
		float heading = headingVec.Dot(myCharacter->GetActorForwardVector());

		float headingToDegrees = UKismetMathLibrary::RadiansToDegrees(UKismetMathLibrary::Acos(heading));

		//Check if the player is in the correct angle
		//Divide by two to make it easier in editor to say they have 180 degrees vision infront of them for example
		if (headingToDegrees <= (myCharacter->sightAngle / 2))
		{
			objectToTrack = playerCharacter;
			myCharacter->bShouldHeadTrack = true;
		}
		else
		{
			objectToTrack = nullptr;//Later check for if we are overlapping an interactable
			myCharacter->bShouldHeadTrack = false;
		}
	}
}


void ABaseNPCController::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == playerCharacter && OtherComp->GetName() == "UI Activator")//Enable tick and try to head track the player
	{
		bShouldCheckForHeadTracking = true;
		SetActorTickEnabled(true);
	}
	else if(!myCharacter->bShouldHeadTrack)
	{
		UInteractComponent* curActorComp = Cast<UInteractComponent>(OtherActor->GetComponentByClass(UInteractComponent::StaticClass()));
		if (IsValid(curActorComp))
		{
			myCharacter->bShouldHeadTrack = true;
			objectToTrack = OtherActor;
		}
	}
}

void ABaseNPCController::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == playerCharacter && OtherComp->GetName() == "UI Activator")//Stop trying to track the player
	{
		bShouldCheckForHeadTracking = false;
		myCharacter->bShouldHeadTrack = false;
		SetActorTickEnabled(false);
	}
	else if(!bShouldCheckForHeadTracking && objectToTrack != nullptr)
	{
		objectToTrack = nullptr;
		myCharacter->bShouldHeadTrack = false;
	}
}

void ABaseNPCController::findScheduledZone(bool getLastLocation)
{
	//Grab the current days schedule from the NPC
	myBlackboard->SetValueAsBool("bIsMoving", true);
	const UDataTable* curScheduleTable = myCharacter->weeklySchedule;
	int curDay = myCharacter->getDayOfTheWeek();
	FName curSeason = myCharacter->getSeason();
	FString rowKey = curSeason.ToString() + FString::FromInt(curDay);
	FAIScheduleRow* daySchedule = curScheduleTable->FindRow<FAIScheduleRow>(FName(*rowKey), "");
	if (daySchedule == nullptr)
	{
		return;
	}

	//Grab the locations and edit the map if its storming
	TMap<int, FScheduledLocationStruct> schedule = daySchedule->normalLocations;
	if (myCharacter->IsStorming())
	{
		TMap<int, FScheduledLocationStruct> stormSchedule = daySchedule->stormingLocations;
		TArray<int> keys;
		stormSchedule.GetKeys(keys);


		for (int key : keys)
		{
			schedule.Add(key, stormSchedule[key]);
		}
	}

	//Grab time of day
	int hour = myCharacter->getHourOfDay();

	if (getLastLocation)
	{
		int lastListedTime = -1;
		TArray<int> keys;
		schedule.GetKeys(keys);
		for (int key : keys)
		{
			if (key < hour)
			{
				lastListedTime = key;
			}
			else if (key > hour)
			{
				break;
			}
		}

		if(lastListedTime != -1)
		{
			FVector newLocation = schedule[lastListedTime].location;
			myBlackboard->SetValueAsFloat("wanderRange", schedule[lastListedTime].wanderRange);
			myBlackboard->SetValueAsVector("SchedueledLocation", newLocation);
			myBlackboard->SetValueAsBool("bMoveToSchedduledZone", true); 
			myBlackboard->SetValueAsBool("bShouldTeleportToZone", true);
		}
		else
		{
			UKismetSystemLibrary::PrintWarning("Time = -1");
			myBlackboard->SetValueAsBool("bMoveToSchedduledZone", false);
			myBlackboard->SetValueAsBool("bIsMoving", false);
		}
	}
	else if (schedule.Contains(hour))
	{
		FVector newLocation = schedule[hour].location;
		myBlackboard->SetValueAsFloat("wanderRange", schedule[hour].wanderRange);
		myBlackboard->SetValueAsVector("SchedueledLocation", newLocation);
		myBlackboard->SetValueAsBool("bMoveToSchedduledZone", true);
	}
	else
	{
		myBlackboard->SetValueAsBool("bMoveToSchedduledZone", false);
		myBlackboard->SetValueAsBool("bIsMoving", false);
	}
}

//NPC interacting WITH things
bool ABaseNPCController::tryInteract()
{
	TArray<AActor*> interactables;
	mySphereComponent->GetOverlappingActors(interactables);

	for (int i = 0; i < interactables.Num(); ++i)
	{
		UInteractComponent* curActorComp = Cast<UInteractComponent>(interactables[i]->GetComponentByClass(UInteractComponent::StaticClass()));

		if (IsValid(curActorComp) && myCharacter->interactionTypes.Contains(FName(*curActorComp->interactionType)))
		{
			myBlackboard->SetValueAsBool("bIsInteracting", true);

			myCharacter->curInteractionType = FName(*curActorComp->interactionType);

			if (curActorComp->GetClass()->ImplementsInterface(UInteractInterface::StaticClass()))
			{
				IInteractInterface::Execute_moveActorIntoPlace(curActorComp, myCharacter);
			}

			return true;
		}
	}
	return false;
}