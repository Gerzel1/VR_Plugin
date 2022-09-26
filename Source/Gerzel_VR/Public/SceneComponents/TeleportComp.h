// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BeamEffects/Parent/Beam_Parent.h"
#include "Components/SceneComponent.h"
#include "Teleport/Parent/TeleportActor_Parent.h"
#include "Teleport/Parent/TeleportLocationIcon_Parent.h"
#include "TeleportComp.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class GERZEL_VR_API UTeleportComp : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTeleportComp();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	//Spawning objects
	UFUNCTION(BlueprintNativeEvent ,BlueprintCallable, Category = "VR | Teleportation")
	void SpawnObjects();

	virtual void SpawnObjects_Implementation();

	UFUNCTION(BlueprintNativeEvent ,BlueprintCallable, Category = "VR | Teleportation")
	void SpawnBeamClass();

	virtual void SpawnBeamClass_Implementation();

	UFUNCTION(BlueprintNativeEvent ,BlueprintCallable, Category = "VR | Teleportation")
	void SpawnTeleportIcon();

	virtual void SpawnTeleportIcon_Implementation();

	//teleportation
	UFUNCTION(BlueprintNativeEvent ,BlueprintCallable, Category = "VR | Teleportation")
	void BeginTeleport(APawn* PawnToTeleport);

	virtual void BeginTeleport_Implementation(APawn* PawnToTeleport);

	UFUNCTION(BlueprintNativeEvent ,BlueprintCallable, Category = "VR | Teleportation")
	void TryToTeleport();

	virtual void TryToTeleport_Implementation();

	UFUNCTION(BlueprintNativeEvent ,BlueprintCallable, Category = "VR | Teleportation")
	void TeleportPawn();

	virtual void TeleportPawn_Implementation();

	UFUNCTION(BlueprintNativeEvent ,BlueprintCallable, Category = "VR | Teleportation")
	void TeleportVis();

	virtual void TeleportVis_Implementation();

	UFUNCTION(BlueprintNativeEvent ,BlueprintCallable, Category = "VR | Teleportation")
	void HideTeleportVis();

	virtual void HideTeleportVis_Implementation();

	UFUNCTION(BlueprintNativeEvent ,BlueprintCallable, Category = "VR | Teleportation")
	void TraceResult(FHitResult Hit);

	virtual void TraceResult_Implementation(FHitResult Hit);

	UFUNCTION(BlueprintCallable, Category = "VR | Teleportation")
	FHitResult LineTrace(USceneComponent* SceneComp, FVector Offset, float TraceDist);

	//server events
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "VR | Teleportation")
	void Server_BeginTeleport(bool const bTryingToTeleport, APawn* PawnToTeleport);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "VR | Teleportation")
	void Server_TeleportPawn();

private:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "VR | Teleportation")
	bool CollectionParamValid();

	UFUNCTION(BlueprintCallable, Category = "VR | Teleportation")
	void TeleportLogic();
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "True"), Category = "VR | Spawnable")
	AActor* TeleportBeam;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "True"), Category = "VR | Spawnable")
	TSubclassOf<ABeam_Parent> TeleportBeamClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "True"), Category = "VR | Spawnable")
	AActor* TeleportIcon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "True"), Category = "VR | Spawnable")
	TSubclassOf<ATeleportLocationIcon_Parent> TeleportIconClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "True"), Category = "VR | Spawnable")
	FVector TeleportOffsetLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "True"), Category = "VR | Spawnable")
	float TraceDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "True"), Category = "VR | Teleportation")
	float TraceExtent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "True"), Replicated, Category = "VR | Teleportation")
	bool bTryingToTele;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "True"), Replicated, Category = "VR | Teleportation")
	bool bValidTeleportLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "True"), Replicated, Category = "VR | Teleportation")
	FVector TraceEnd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "True"), Replicated, Category = "VR | Teleportation")
	FVector TraceStart;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "True"), Replicated, Category = "VR | Teleportation")
	APawn* Pawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "True"), Replicated, Category = "VR | Teleportation")
	UMaterialParameterCollection* MaterialCollection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "True"), Replicated, Category = "VR | Teleportation")
	FName ParamName;
};
