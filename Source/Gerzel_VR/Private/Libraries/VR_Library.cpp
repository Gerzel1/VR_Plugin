// Fill out your copyright notice in the Description page of Project Settings.

#include "Libraries/VR_Library.h"
#include "Kismet/KismetSystemLibrary.h"


FHitResult UVR_Library::SimpleLineTrace(USceneComponent* SceneComp, FVector Offset, float TraceDistance)
{
	FHitResult Hit;
	
	if(SceneComp)
	{
		FVector Start = SceneComp->GetComponentLocation()+Offset;
		FVector End = (SceneComp->GetForwardVector()* TraceDistance) + Start;
		
		GetWorld()->LineTraceSingleByChannel(Hit, Start, End,ECC_Visibility);

		return Hit;
	}
	return Hit;
}
