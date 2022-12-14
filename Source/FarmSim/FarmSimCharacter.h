// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "../../Plugins/SimpleInteract/Source/SimpleInteract/Public/InteractComponent.h"
#include "GameFramework/Character.h"
#include "PlayerStatus.h"
#include "LocationStatus.h"
#include "ToolItem.h"
#include "InventoryComponent.h"
#include "PlayerSaveManagerComponent.h"
#include "FarmSimCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStatusChange, PlayerStatus, newStatus);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerToolChange, PlayerToolStatus, newStatus);

UCLASS(config=Game)
class AFarmSimCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	AFarmSimCharacter();

	virtual void Tick(float DeltaTime) override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	float TurnRate = 1.25f;


	/*INTERACTION*/
	UPROPERTY(BlueprintReadWrite)
	UInteractComponent* interactActorComp = nullptr;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnPlayerStatusChange PlayerStatusChanged;
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnPlayerToolChange PlayerToolChanged;



	//Input functions

	/** Called for forwards/backward input */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void MoveForwardAction(float Value);

	/** Called for side to side input */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void MoveRightAction(float Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CameraUpAction(float Rate);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CameraRightAction(float Rate);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void JumpAction(bool isJumping);

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
	void OpenInventoryAction(bool open = false, bool toggle = true);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void changeEquippedTool(PlayerToolStatus newTool);
	//


	UFUNCTION(BlueprintImplementableEvent)
	void setSelectedItem(bool showUI = true);

	UFUNCTION(BlueprintCallable)
	void setPlayerStatus(PlayerStatus newStatus);


	//Tool stuff
	UFUNCTION(BlueprintCallable)
	void changeTool(FName toolType, FToolInvItem newTool);

	UFUNCTION(BlueprintCallable)
	FToolInvItem grabTool(FName toolType);
	//

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	PlayerToolStatus getEquippeddTool() { return toolStatus; };
	PlayerStatus getPlayerStatus() { return curPlayerStatus; };

	//UI Functions
	UFUNCTION(BlueprintImplementableEvent)
	void displayNotification(const FString& message, int showLength = 2);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<PlayerStatus> curPlayerStatus = PlayerStatus::NormalState;
	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<PlayerStatus> prevPlayerStatus = PlayerStatus::NormalState;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<PlayerToolStatus> toolStatus = PlayerToolStatus::NoToolOut;
	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<PlayerToolStatus> prevToolStatus = PlayerToolStatus::NoToolOut;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<LocationStatus> curPlayerLocation = LocationStatus::Valley;
	UFUNCTION(BlueprintCallable)
	LocationStatus getPlayerLocation() { return curPlayerLocation; };
	UFUNCTION(BlueprintCallable)
	void setPlayerLocation(LocationStatus newLoc) { curPlayerLocation = newLoc; };

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
	FHitResult placementLineTraceDown(bool snapToGrid = true, bool drawDebug = false, float distanceMod = 1);

	AActor* placementPreview;
	//

	UPROPERTY(BlueprintReadWrite)
	int curSelectedSeedSlot = -2;

	UPROPERTY(BlueprintReadWrite)
	UInventoryComponent* myInventoryComp = nullptr;

	//Tool stuff
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FToolInvItem> currentTools;

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
	void fishingMiniGameDelegateFunc(bool Status, FInvItem FishCaught);

	UFUNCTION(BlueprintCallable)
	bool checkForPhysMat(FVector location, EPhysicalSurface surfaceToCheckFor);
	//

	//Save game
	UPlayerSaveManagerComponent* mySaveManager;
};

