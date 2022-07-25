// Fill out your copyright notice in the Description page of Project Settings.


#include "Teleport/Parent/TeleportActor_Parent.h"

// Sets default values
ATeleportActor_Parent::ATeleportActor_Parent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATeleportActor_Parent::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATeleportActor_Parent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATeleportActor_Parent::TeleportHit_Implementation(FVector Location, bool IsValidHit)
{
}

