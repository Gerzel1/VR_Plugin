// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VR_Character_Parent.generated.h"

class UCameraComponent;
class USceneComponent;
class UStaticMeshComponent;
class UMotionControllerComponent;
class AVR_Hands_Parent;
class APlayerController;


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
		void CameraOffset();

		virtual void CameraOffset_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR | Camera")
		void SetCameraToFloor();

		virtual void SetCameraToFloor_Implementation();

protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR | Teleportation")
		void ScanToTeleport(USceneComponent* TraceLineFromHere);

		virtual void ScanToTeleport_Implementation(USceneComponent* TraceLineFromHere);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR | Teleportation")
		void ScanForTeleportLocation();

		virtual void ScanForTeleportLocation_Implementation();

	UFUNCTION(Category = "VR | Teleportation")
		void TeleportLeftHand();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR | Teleportation")
		void StopTryingToTeleport();

		virtual void StopTryingToTeleport_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR | Teleportation")
		void TeleportUser();

		virtual void TeleportUser_Implementation();

protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR | UserInfo")
		void SavePlayerController();

	virtual void SavePlayerController_Implementation();


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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VR | Hands")
		UMotionControllerComponent* LeftHandRoot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VR | Hands")
		UMotionControllerComponent* RightHandRoot;

protected:
	UPROPERTY(VisibleAnywhere, Category = "VR | Teleportation")
		UStaticMeshComponent* TeleportMesh;

	UPROPERTY(EditAnywhere, Category = "VR | Teleportation")
		float MaxTeleportRange = 1000.f;

	UPROPERTY(EditAnywhere, Category = "VR | Teleportation")
		FVector TeleportOffset = FVector(0.0f, 0.0f, 10.0f);

	UPROPERTY(VisibleAnywhere, Category = "VR | Teleportation")
		bool bTryingToTeleport;

	UPROPERTY(VisibleAnywhere, Category = "VR | Teleportation")
		bool bValidTeleportLocation;

	UPROPERTY(VisibleAnywhere, Category = "VR | Teleportation")
		FVector TeleportLocation; 

	UPROPERTY(EditAnywhere, Category = "VR | Teleportation")
		float TeleportFadeTime = 0.25f;

	UPROPERTY(EditAnywhere, Category = "VR | Teleportation")
		USceneComponent* TraceFromHere;

	UPROPERTY(EditAnywhere, Category = "VR | Teleportation")
		FVector TeleportProjectionExtent = FVector(100, 100, 100);
	
protected:
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bUseVRHands"), Category = "VR | Hands")
		TSubclassOf<AVR_Hands_Parent> VR_Hand_Left;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bUseVRHands"), Category = "VR | Hands")
		TSubclassOf<AVR_Hands_Parent> VR_Hand_Right;

protected: 
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bUseVRHands"), Category = "VR | UserInfo")
		APlayerController* OwningPlayerController;
	
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
