// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/BasicVrPawn.h"

ABasicVrPawn::ABasicVrPawn()
{
	CharacterRoot = CreateDefaultSubobject<USceneComponent>(TEXT("CharacterRoot"));
	CharacterRoot->SetupAttachment(VRRoot);

	BodyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("BodyRoot"));
	BodyRoot->SetupAttachment(CharacterRoot);

	HeadRoot = CreateDefaultSubobject<USceneComponent>(TEXT("HeadRoot"));
	HeadRoot->SetupAttachment(CharacterRoot);
}

void ABasicVrPawn::GetBodyMovementTransforms_Implementation()
{
}
