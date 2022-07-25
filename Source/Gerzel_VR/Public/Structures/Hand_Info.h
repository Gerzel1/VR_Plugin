// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Hand_Info.generated.h"

USTRUCT(BlueprintType)
struct FFingerStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fingers")
	float BaseJoint_Y = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fingers")
	float BaseJoint_Z = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fingers")
	float MiddleJoint_Y = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fingers")
	float EndJoint_Y = 0;
	
	UPROPERTY()
	UObject* SafeObjectPointer;
	
};

USTRUCT(BlueprintType)
struct FHandStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fingers")
	FFingerStruct IndexFinger;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fingers")
	FFingerStruct MiddleFinger;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fingers")
	FFingerStruct RingFinger;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fingers")
	FFingerStruct PinkyFinger;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fingers")
	FFingerStruct Thumb;

	UPROPERTY()
	UObject* SafeObjectPointer;
};
