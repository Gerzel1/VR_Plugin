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
		void CameraOffset();

		virtual void CameraOffset_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR | Camera")
		void SetCameraToFloor();

		virtual void SetCameraToFloor_Implementation();

protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR | Teleportation")
		void ScanToTeleport(USceneComponent* TraceLineFromHere);

		virtual void ScanToTeleport_Implementation(USceneComponent* TraceLineFromHere);

	UFUNCTION(Server, Reliable, BlueprintCallable)
		void Server_ScanToTeleport(USceneComponent* TraceLineFromHere);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR | Teleportation")
		void ScanForTeleportLocation();

		virtual void ScanForTeleportLocation_Implementation();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR | Teleportation")
		void DrawTeleportLine();

		virtual void DrawTeleportLine_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR | Teleportation")
		void TeleportLeftHand();

		virtual void TeleportLeftHand_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR | Teleportation")
		void StopTryingToTeleport();

		virtual void StopTryingToTeleport_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR | Teleportation")
		void StopTryingToTeleportLeft();

		virtual void StopTryingToTeleportLeft_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR | Teleportation")
		void TeleportUser();

		virtual void TeleportUser_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR | Teleportation")
		void SpawnTeleportIcon();

		virtual void SpawnTeleportIcon_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR | Teleportation")
		void SpawnTeleportBeam();

		virtual void SpawnTeleportBeam_Implementation();

	UFUNCTION(Server, Reliable, BlueprintCallable)
		void Server_TeleportUser();

protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR | UserInfo")
		void SavePlayerController();

		virtual void SavePlayerController_Implementation();

protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR | Character")
		FVector GetBottomOfCharacter();

		virtual FVector GetBottomOfCharacter_Implementation();


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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR | Teleportation")
		TSubclassOf<ATeleportLocationIcon_Parent> TeleportLocationIconClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "VR | Teleportation")
		ATeleportLocationIcon_Parent* TeleportLocationIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR | Teleportation")
		TSubclassOf<ABeam_Parent> TeleportBeamClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "VR | Teleportation")
		ABeam_Parent* TeleportBeam;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR | Teleportation")
		float MaxTeleportRange = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR | Teleportation")
		FVector TeleportOffset = FVector(0.0f, 0.0f, 10.0f);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "VR | Teleportation")
		bool bTryingToTeleport;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "VR | Teleportation")
		bool bValidTeleportLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "VR | Teleportation")
		FVector TeleportLocation; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR | Teleportation")
		float TeleportFadeTime = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "VR | Teleportation")
		USceneComponent* TraceFromHere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR | Teleportation")
		FVector TeleportProjectionExtent = FVector(100, 100, 100);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "VR | Teleportation")
		FVector TeleportEnd;
	
protected:
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bUseVRHands"), Category = "VR | Hands")
		TSubclassOf<AVR_Hands_Parent> VR_Hand_Left;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bUseVRHands"), Category = "VR | Hands")
		TSubclassOf<AVR_Hands_Parent> VR_Hand_Right;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bUseVRHands"), Category = "VR | Hands")
		bool bTeleportLeftHand;

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
