// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TeleportActor_Parent.generated.h"

UENUM()
enum ETeleportActorType
{
	Default UMETA(DisplayName = "Default"),
	Point	UMETA(DisplayName = "TeleportPoint")
};

UCLASS()
class GERZEL_VR_API ATeleportActor_Parent : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATeleportActor_Parent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR | Teleportation")
	void TeleportHit(FVector Location, bool IsValidHit);

	virtual void TeleportHit_Implementation(FVector Location, bool IsValidHit);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR | Teleportation")
	TEnumAsByte<ETeleportActorType> TeleportActorType;
};
