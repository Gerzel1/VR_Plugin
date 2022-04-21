// Fill out your copyright notice in the Description page of Project Settings.


#include "BeamEffects/Parent/Beam_Parent.h"

// Sets default values
ABeam_Parent::ABeam_Parent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABeam_Parent::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABeam_Parent::DrawBeam_Implementation(FVector Start, FVector End, bool bValidHit)
{
}

// Called every frame
void ABeam_Parent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

