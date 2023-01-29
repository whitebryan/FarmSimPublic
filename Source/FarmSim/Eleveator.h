// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h" 
#include "Eleveator.generated.h"

UCLASS()
class FARMSIM_API AElevator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AElevator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	//Components
	USceneComponent* testMove;
	UStaticMeshComponent* elevatorBody;
	USplineComponent* splineToFollow;

	UPROPERTY(EditAnywhere, category = "Movement", Meta = (tooltip = "Speed to move between floors, if you go too fast it wont be able to stop"))
	float elevatorSpeed = 200;

	int targetFloor = 0;
	int curFloor = 0;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void moveToFloor(int floor);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void changeTextColorAndStatus(FName status, int floor);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void toggleInteractability();

};
