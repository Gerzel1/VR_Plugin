// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pawns/Parent/VR_Character_Parent.h"
#include "BasicVrPawn.generated.h"

/**
 * 
 */
UCLASS()
class GERZEL_VR_API ABasicVrPawn : public AVR_Character_Parent
{
	GENERATED_BODY()
public:
	ABasicVrPawn();
	

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR | Character")
	USceneComponent* CharacterRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR | Character")
	USceneComponent* BodyRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR | Character")
	USceneComponent* HeadRoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR | Character")
	FRotator LastBodyRot;

protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR | Character")
	void GetBodyMovementTransforms();

	virtual void GetBodyMovementTransforms_Implementation();
};
