// Fill out your copyright notice in the Description page of Project Settings.


#include "Hands/Parent/Hands_Parent.h"

// Sets default values
AHands_Parent::AHands_Parent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHands_Parent::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHands_Parent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

