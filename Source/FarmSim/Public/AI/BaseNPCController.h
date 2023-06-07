// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Engine/DataTable.h"
#include "../../Plugins/SimpleInteract/Source/SimpleInteract/Public/InteractComponent.h"
#include "BaseAICharacter.h"
#include "Components/SphereComponent.h"
#include "BehaviorTree/BlackboardComponent.h" 
#include "BaseNPCController.generated.h"

/**
 * 
 */
UCLASS()
class FARMSIM_API ABaseNPCController : public AAIController, public IInteractInterface
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	UBlackboardComponent* myBlackboard;

	bool bShouldCheckForHeadTracking = false;

	UPROPERTY(BlueprintReadWrite)
	AActor* objectToTrack;

protected:
	ABaseNPCController();

	AActor* playerCharacter;
	ABaseAICharacter* myCharacter;
	USphereComponent* mySphereComponent;

	int curConversation = 0;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void findScheduledZone(bool getLastLocation);

	UFUNCTION(BlueprintCallable)
	bool tryInteract();
};
