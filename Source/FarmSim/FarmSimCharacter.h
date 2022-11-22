// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "../../Plugins/SimpleInteract/Source/SimpleInteract/Public/InteractComponent.h"
#include "GameFramework/Character.h"
#include "PlayerStatus.h"
#include "InventoryComponent.h"
#include "FarmSimCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStatusChange, PlayerStatus, newStatus);

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

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	float TurnRate = 1.25f;


	/*INTERACTION*/
	UPROPERTY(BlueprintReadWrite)
	UInteractComponent* interactActorComp = nullptr;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnPlayerStatusChange PlayerStatusChanged;



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
	void ScrollItemsAction(float Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void EscMenuAction();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OpenInventoryAction();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void changeEquippedTool(PlayerToolStatus newTool);
	//



	UFUNCTION(BlueprintImplementableEvent)
	void setSelectedItem(bool showUI = true);

	UFUNCTION(BlueprintCallable)
	void setPlayerStatus(PlayerStatus newStatus);

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<PlayerStatus> curPlayerStatus = PlayerStatus::NormalState;
	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<PlayerStatus> prevPlayerStatus = PlayerStatus::NormalState;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<PlayerToolStatus> toolStatus = PlayerToolStatus::NoToolOut;

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
	//

	UPROPERTY(BlueprintReadWrite)
	int curSelectedSeedSlot = -2;

	UPROPERTY(BlueprintReadWrite)
	UInventoryComponent* myInventoryComp = nullptr;
};

