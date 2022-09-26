// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/TeleportInterface.h"
#include "Beam_Parent.generated.h"


UCLASS(Abstract)
class GERZEL_VR_API ABeam_Parent : public AActor, public ITeleportInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABeam_Parent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR | Teleportation")
		void DrawBeam(FVector Start, FVector End, bool bValidHit);

	virtual void DrawBeam_Implementation(FVector Start, FVector End, bool bValidHit);

public:
	void TeleportTraceResult_Implementation(FVector Start, FVector End, bool ValidHit) override;
	
};
