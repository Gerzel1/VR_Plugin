// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Structures/Hand_Info.h"
#include "VR_Hands_Anim.generated.h"

/**
 * 
 */
UCLASS()
class GERZEL_VR_API UVR_Hands_Anim : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR | Hands")
		bool bTryingToGrab = false;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR | Hands")
        FHandStruct HandStruct;
};
