// Fill out your copyright notice in the Description page of Project Settings.


#include "Hands/VR_Hand_Parent.h"

// Sets default values
AVR_Hand_Parent::AVR_Hand_Parent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void AVR_Hand_Parent::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVR_Hand_Parent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVR_Hand_Parent::AttachHands_Implementation(AActor* Actor, USceneComponent* SceneComponent, EAttachmentRule AttachRule)
{
	if(!Actor || !SceneComponent) return;
	FAttachmentTransformRules const AttachmentRules(AttachRule, AttachRule, AttachRule, false);
	Actor->AttachToComponent(SceneComponent, AttachmentRules);
}

void AVR_Hand_Parent::Multicast_AttachHands_Implementation(AActor* Actor, USceneComponent* SceneComponent, EAttachmentRule AttachRule)
{
	if(!Actor || !SceneComponent) return;
	AttachHands(Actor, SceneComponent, AttachRule);
}

