// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VR_Character_Parent.generated.h"

class UCameraComponent;
class USceneComponent;
class UStaticMeshComponent;
class UMotionControllerComponent;
class AVR_Hand_Parent;
class APlayerController;
class ATeleportLocationIcon_Parent;
class ABeam_Parent;


UCLASS()
class GERZEL_VR_API AVR_Character_Parent : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVR_Character_Parent();

protected:

	virtual void OnConstruction(const FTransform& Transform) override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR | Camera")
		void SetCameraToFloor();

		virtual void SetCameraToFloor_Implementation();

protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR | Character")
		FVector GetBottomOfCharacter();

		virtual FVector GetBottomOfCharacter_Implementation();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "VR | Character")
		void Server_UpdateHandsAndHead(FTransform Head, FTransform Left, FTransform Right);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR | Character")
		void UpdateHandsAndHead(FTransform Head, FTransform Left, FTransform Right);

		virtual void UpdateHandsAndHead_Implementation(FTransform Head, FTransform Left, FTransform Right);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR | Character")
		void GetBodyMovementTransforms();

		virtual void GetBodyMovementTransforms_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR | Character")
	void UpdateBodyMovement();

	virtual void UpdateBodyMovement_Implementation();

protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR | VR_Info")
	bool IsVREnabled();

protected:
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "VR | Hands")
	void Server_SpawnHands();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR | Hands")
	void SpawnHands();

	virtual void SpawnHands_Implementation();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VR | Camera")
		UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VR | Camera")
		USceneComponent* VRRoot;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bUseVRHands"), Replicated, Category = "VR | Hands")
		TSubclassOf<AVR_Hand_Parent> VR_Hand_Left;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bUseVRHands"), Replicated, Category = "VR | Hands")
		TSubclassOf<AVR_Hand_Parent> VR_Hand_Right;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bUseVRHands"), Category = "VR | Hands")
		AVR_Hand_Parent* LeftHand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bUseVRHands"), Category = "VR | Hands")
	AVR_Hand_Parent* RightHand;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bUseVRHands"), Category = "VR | Hands")
		bool bTeleportLeftHand;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VR | Hands")
		UMotionControllerComponent* LeftHandRoot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VR | Hands")
		UMotionControllerComponent* RightHandRoot;

	//UPROPERTY(BlueprintReadOnly, Category = "VR")
		//IHeadMountedDisplay* pHMD;

protected: 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR | UserInfo")
		APlayerController* OwningPlayerController;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "VR | Character")
		FTransform HeadTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "VR | Character")
		FTransform LeftHandTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "VR | Character")
		FTransform RightHandTransform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR | Character")
		USceneComponent* CharacterRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR | Character")
		USceneComponent* BodyRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR | Character")
		USceneComponent* HeadRoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR | Character")
		FRotator LastBodyRot;
	
private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "True"), Category = "VR | Class Defaults")
		bool bUseVRHands = true;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "True"), Category = "VR | Class Defaults")
		bool bUseVRTeleport = true;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "True"), Category = "VR | Debug")
		bool bDebugMode = false;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "True"), Category = "VR | Debug | Teleportation", meta = (EditCondition = "bDebugMode"))
		bool bDebugTeleportation = false;
};
