// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "../../Plugins/SimpleInteract/Source/SimpleInteract/Public/InteractComponent.h"
#include "GameFramework/Character.h"
#include "ToolItem.h"
#include "InventoryComponent.h"
#include "PlayerSaveManagerComponent.h"
#include "Containers/Queue.h"
#include "Templates/Tuple.h"
#include "GameplayTagAssetInterface.h" 
#include "GameplayTagContainer.h"
#include "PlayerUIInterface.h"
#include "BaseCharacter.h"
#include "FarmSimCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStatusChange, FGameplayTag, newStatus);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerToolChange, FGameplayTag, newStatus);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFishingRecordChange, FFishRecordStruct, newRecord);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFishCaught, UFishItemAsset*, fishCaught);

UCLASS(config=Game)
class AFarmSimCharacter : public ABaseCharacter, public IGameplayTagAssetInterface, public IPlayerUIInterface
{
	GENERATED_BODY()

public:
	AFarmSimCharacter();

	UFUNCTION(BlueprintCallable)
	void setPlayerStatus(FGameplayTag newStatus);
	UFUNCTION(BlueprintCallable)
	virtual void GetOwnedGameplayTags(FGameplayTagContainer &TagContainer) const override;//; virtual void GetOwnedGameplayTags_Implementation(FGameplayTagContainer& TagContainer) const override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer playerTags;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer movementAllowableTags;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer cameraRestrictingTags;

	virtual void Tick(float DeltaTime) override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Config, Category=Input)
	float TurnRate = 1.25f;

	UPROPERTY(BlueprintReadWrite)
	AActor* objectToTrack;

	UPROPERTY(BlueprintReadWrite)
	UInventoryComponent* myInventoryComp = nullptr;

	/*INTERACTION*/
	UPROPERTY(BlueprintReadWrite)
	UInteractComponent* interactActorComp = nullptr;
	UPROPERTY(BlueprintReadWrite)
	TArray<UInteractComponent*> otherInteractComps;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<FName, FString> interactionPrompts;
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void updateInteractPrompt(bool visibility = true);
	//


	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnPlayerStatusChange PlayerStatusChanged;
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnPlayerToolChange PlayerToolChanged;
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnFishingRecordChange newFishingRecordChange;
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnFishCaught caughtFish;



	//Input functions
	
	//Overrides of parent functions
	void MoveAction(FVector2D Value) override;
	void CameraAction(FVector2D Value) override;
	void JumpAction(bool isJumping) override; 

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SprintAction(float Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void WalkAction(float Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InteractAction();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UseToolAction();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void FishingCastAction(bool pressed);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ScrollItemsAction(float Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void EscMenuAction();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void changeEquippedTool(FGameplayTag newTool);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void togglePlacementModeAction();
	//


	UFUNCTION(BlueprintImplementableEvent)
	void setSelectedItem(bool showUI = true);


	//Tool stuff
	UFUNCTION(BlueprintCallable)
	void changeTool(UToolItemAsset* newTool);

	UFUNCTION(BlueprintCallable)
	UToolItemAsset* grabTool(FGameplayTag toolTag);

	//UI Functions
	UFUNCTION(BlueprintCallable)
	void displayNotification(const FString& message, int showLength = 2);
	UFUNCTION(BlueprintImplementableEvent)
	void newNotification(const FString& message, int showLength = 2);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void toggleMenuUI(bool bStatus, const FName menuToOpenTo = "None", bool bShouldToggle = false);
	void setPlayerUI(bool bStatus, const FName menuToOpenTo = "None", bool bShouldToggle = false); virtual void setPlayerUI_Implementation(bool bStatus, const FName menuToOpenTo, bool bShouldToggle) override;


	//Tool animation Variables
	UPROPERTY(BlueprintReadOnly, Category = "Tool animation Vars")
	int lookingHeight = 1;
	UPROPERTY(BlueprintReadWrite, Category = "Tool animation Vars")
	bool toolUsed = false;
	UStaticMeshComponent* toolMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FGameplayTag, UToolItemAsset*> currentTools;

	UFUNCTION(BlueprintImplementableEvent)
	void changeClothingPiece(UModularClothingAsset* newPiece, FLinearColor pieceColor);



	FGameplayTag locationTag;
	FGameplayTag playerStatusTag;
	FGameplayTag toolStatusTag;

	UFUNCTION(BlueprintCallable)
	void changeTag(FGameplayTag newTag);
	UFUNCTION(BlueprintCallable)
	FGameplayTag findTagOfType(FGameplayTag parentTag);

	UFUNCTION(BlueprintImplementableEvent)
	void conversationControl(const FString& convoCommand = "Next");

	UFUNCTION(BlueprintCallable)
	void setSensitivity(float newSensPercent);
	UFUNCTION(BlueprintCallable)
	float getSensitivity();

	UFUNCTION(BlueprintCallable)
	void setFOV(int newFOV);
	UFUNCTION(BlueprintCallable)
	int getFOV();

protected:
	virtual void BeginPlay() override;


	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//Movement
	UPROPERTY(EditAnywhere)
	float normalSpeed = 350;
	UPROPERTY(EditAnywhere)
	float walkSpeed = 200;
	UPROPERTY(EditAnywhere)
	float runSpeed = 500;

	//Player Statuses
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag prevPlayerStatus = FGameplayTag::EmptyTag;

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag prevToolStatus = FGameplayTag::EmptyTag;


	//Grid snapping and item placement
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Placement", meta = (ToolTip = "What number to round towards for grid snapping"))
	int gridSnap = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Placement")
	int digDistance = 50;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Placement")
	int maxSlope = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Placement")
	TSubclassOf<class AActor> growthPlotActor;
	int growthPlotZExtent = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Placement")
	TSubclassOf<class AActor> growthPlotPreview;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Placement")
	TSubclassOf<class AActor> wateringCanPreview;

	UFUNCTION(BlueprintCallable)
	FHitResult placementLineTrace(bool fromCamera = true, bool snapToGrid = true, bool drawDebug = false, float distanceMod = 1, float distanceDown = 300);
	UFUNCTION(BlueprintCallable)
	FHitResult placementLineTraceDown(FVector point, bool snapToGrid = true, bool drawDebug = false, float distanceMod = 1, float distanceDown = 300);

	AActor* placementPreview;
	AActor* lastHighlighted;

	UPROPERTY(EditAnywhere)
	UDataTable* placeablesTable;
	void placePlaceable(bool place = true);
	//

	UPROPERTY(BlueprintReadWrite)
	int curSelectedItemSlot = -2;

	//Fishing Stuff'
	UPROPERTY(EditAnywhere, Category = "Fishing")
	TSubclassOf<AActor> fishingIndicatorBlueprint;
	UPROPERTY(EditAnywhere, Category = "Fishing")
	TSubclassOf<AActor> fishingMiniGameBlueprint;
	AActor* curFishingIndicator;
	UPROPERTY(EditAnywhere, Category = "Fishing", meta = (ClampMin = "5", ClampMax = "50", UMin = "5", UMax = "50", Tooltip ="How far the player can cast while fishing"))
	float maxCastDistance = 20;
	float curDistance = 1;
	AActor* curFishingMiniGame;
	UFUNCTION()
	void fishingMiniGameDelegateFunc(bool Status, const FInvItem& FishCaught);
	UFUNCTION()
	void fishingRecordDelegateFunc(FFishRecordStruct fishRecord);

	UFUNCTION(BlueprintCallable)
	bool checkForPhysMat(FVector location, EPhysicalSurface surfaceToCheckFor);
	//

	//Save game
	UPlayerSaveManagerComponent* mySaveManager;

	TQueue<TTuple<FString, int>> notificationQueue;
	UPROPERTY(BlueprintReadWrite)
	FString curNotification = "null";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Physics")
	float breastPhysicsStrength = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* myMainMesh;
	UFUNCTION(BlueprintImplementableEvent)
	void reAttachTool(FName socket);
	UFUNCTION(BlueprintImplementableEvent)
	void setOtherInvComp(UInventoryComponent* otherInv);
};

