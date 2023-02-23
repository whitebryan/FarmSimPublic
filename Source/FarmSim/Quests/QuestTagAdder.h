// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QuestComponent.h"
#include "QuestTagAdder.generated.h"

UCLASS()
class FARMSIM_API AQuestTagAdder : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQuestTagAdder();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	FGameplayTag tagToAdd = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere)
	UQuestAsset* requiredInProgressQuest = nullptr;

	UPROPERTY(EditAnywhere)
	bool bDestroyAfterUse = true;
};
