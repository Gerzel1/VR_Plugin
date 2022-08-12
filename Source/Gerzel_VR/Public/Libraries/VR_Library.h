// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VR_Library.generated.h"

//class UWidget;

UCLASS()
class GERZEL_VR_API UVR_Library : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "VR | Trace")
	FHitResult SimpleLineTrace(USceneComponent* SceneComp, FVector Offset, float TraceDistance);
};
