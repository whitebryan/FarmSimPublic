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
class FARMSIM_API ABaseNPCController : public AAIController
{
	GENERATED_BODY()
	
public:


protected:
	ABaseNPCController();

	AActor* playerCharacter;
	ABaseAICharacter* myCharacter;
	USphereComponent* mySphereComponent;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;


	bool bShouldCheckForHeadTracking = false;

	UPROPERTY(BlueprintReadWrite)
	UBlackboardComponent* myBlackboard;

	UPROPERTY(BlueprintReadWrite)
	AActor* objectToTrack;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void findScheduledZone(bool getLastLocation);

	UFUNCTION(BlueprintCallable)
	bool tryInteract();
};
