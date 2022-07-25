// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComps/PickUpComp.h"

// Sets default values for this component's properties
UPickUpComp::UPickUpComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPickUpComp::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UPickUpComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

//Pick up
void UPickUpComp::PickUpItem_Implementation(UPrimitiveComponent* AttachToThis)
{
}

//Server Pick up
void UPickUpComp::Multicast_PickUpItem_Implementation(UPrimitiveComponent* AttachToThis)
{
	PickUpItem(AttachToThis);
}

void UPickUpComp::Server_PickUpItem_Implementation(UPrimitiveComponent* AttachToThis)
{
	Multicast_PickUpItem(AttachToThis);
}

//Drop
void UPickUpComp::DropItem_Implementation()
{
}

//Server Drop
void UPickUpComp::Server_DropItem_Implementation()
{
	Multicast_DropItem();
}

void UPickUpComp::Multicast_DropItem_Implementation()
{
	DropItem();
}

//Interface Events
void UPickUpComp::PickUp_Implementation(UPrimitiveComponent* AttachToThis)
{
	Server_PickUpItem(AttachToThis);
}

void UPickUpComp::Drop_Implementation()
{
	Server_DropItem();
}

