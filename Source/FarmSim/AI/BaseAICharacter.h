// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine/DataTable.h"
#include "BaseAICharacter.generated.h"


USTRUCT(BlueprintType, Blueprintable)

struct FScheduledLocationStruct
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector location = FVector(0,0,0);
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float wanderRange = 300;
};

USTRUCT(BlueprintType, Blueprintable)
struct FAIScheduleRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<int, FScheduledLocationStruct> normalLocations;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<int, FScheduledLocationStruct> stormingLocations;
};

UCLASS()
class FARMSIM_API ABaseAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseAICharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(BlueprintReadOnly)
	bool bShouldHeadTrack = false;

	UPROPERTY(BlueprintReadWrite)
	FName curInteractionType = "None";

	UPROPERTY(BlueprintReadWrite)
	bool bIsWalking = false;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* weeklySchedule;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> interactionTypes;



	//Functions to talk to BP day/night system and weather system
	UFUNCTION(BlueprintImplementableEvent)
	int getDayOfTheWeek() const;

	UFUNCTION(BlueprintImplementableEvent)
	int getHourOfDay() const;

	UFUNCTION(BlueprintImplementableEvent)
	FName getSeason() const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	bool IsStorming();
};
