// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/Parent/VR_Character_Parent.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "MotionControllerComponent.h"
#include "Components/CapsuleComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"
#include "Teleport/Parent/TeleportActor_Parent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Teleport/Parent/TeleportLocationIcon_Parent.h"
#include "Math/UnrealMathUtility.h"
#include "Math/Vector.h"
#include "Kismet/KismetMathLibrary.h"
#include "BeamEffects/Parent/Beam_Parent.h"
#include "IXRTrackingSystem.h"
#include "IHeadMountedDisplay.h"
#include "Hands/VR_Hand_Parent.h"
#include "Engine/EngineTypes.h"


// Sets default values
AVR_Character_Parent::AVR_Character_Parent()
{
	
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);

	VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
	VRRoot->SetupAttachment(GetRootComponent());     

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(VRRoot);
	
	LeftHandRoot = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftHandRoot"));
	LeftHandRoot->SetupAttachment(VRRoot);
	LeftHandRoot->MotionSource = FName(TEXT("Left"));
	RightHandRoot = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightHandRoot"));
	RightHandRoot->SetupAttachment(VRRoot);
	RightHandRoot->MotionSource = FName(TEXT("Right"));
	
	SetActorTickEnabled(false);

	bUseControllerRotationYaw = false;
}

void AVR_Character_Parent::OnConstruction(const FTransform& Transform)
{
	SetCameraToFloor();
}

// Called when the game starts or when spawned
void AVR_Character_Parent::BeginPlay()
{
	Super::BeginPlay();
	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);

	OwningPlayerController = Cast<APlayerController>(GetController());
}

// Called every frame
void AVR_Character_Parent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AVR_Character_Parent::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// Setup Replicated variables
void AVR_Character_Parent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AVR_Character_Parent, VR_Hand_Left);
	DOREPLIFETIME(AVR_Character_Parent, VR_Hand_Right);
	//DOREPLIFETIME_CONDITION(AVR_Character_Parent, HeadTransform, COND_SkipOwner);
	//DOREPLIFETIME_CONDITION(AVR_Character_Parent, LeftHandTransform, COND_SkipOwner);
	//DOREPLIFETIME_CONDITION(AVR_Character_Parent, RightHandTransform, COND_SkipOwner);
}


//Camera
void AVR_Character_Parent::SetCameraToFloor_Implementation()
{
	float ActorHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FVector CameraOffset = VRRoot->GetRelativeLocation();
	CameraOffset.Z = -ActorHalfHeight;
	VRRoot->SetRelativeLocation(CameraOffset);
}


//Character
/*
void AVR_Character_Parent::Server_UpdateHandsAndHead_Implementation(FTransform Head, FTransform Left, FTransform Right)
{
	UpdateHandsAndHead(Head, Left, Right);
}

void AVR_Character_Parent::UpdateHandsAndHead_Implementation(FTransform Head, FTransform Left, FTransform Right)
{
	if (HasAuthority())
	{
		HeadTransform = Head;
		LeftHandTransform = Left;	
		RightHandTransform = Right;
	}
}

void AVR_Character_Parent::GetBodyMovementTransforms_Implementation()
{
	if (IsLocallyControlled())
	{
		HeadTransform = Camera->GetComponentTransform();
		LeftHandTransform = LeftHandRoot->GetComponentTransform();
		RightHandTransform = RightHandRoot->GetComponentTransform();

		Server_UpdateHandsAndHead(HeadTransform, LeftHandTransform, RightHandTransform);
	}
	UpdateBodyMovement();
}

void AVR_Character_Parent::UpdateBodyMovement_Implementation()
{
	if(!IsLocallyControlled())
	{
		LeftHandRoot->SetWorldTransform(LeftHandTransform);
		RightHandRoot->SetWorldTransform(RightHandTransform);
	}
	HeadRoot->SetWorldTransform(HeadTransform);
	
	BodyRoot->SetWorldLocation(HeadTransform.GetLocation() + HeadTransform.GetRotation().GetForwardVector()*-10.0f + HeadTransform.GetRotation().GetUpVector()* -6.0f);

	FRotator const TargetRot = HeadTransform.GetRotation().FQuat::Rotator();

	LastBodyRot = FMath::RInterpTo(LastBodyRot, TargetRot, GetWorld()->GetDeltaSeconds(), 5.0f);

	FRotator const NewRot = FRotator(0.0f, LastBodyRot.Yaw, 0.0f);
	
	CharacterRoot->SetWorldRotation(NewRot);
}
*/
bool AVR_Character_Parent::IsVREnabled_Implementation()
{
	return GEngine->XRSystem.IsValid() && GEngine->XRSystem->GetHMDDevice() && GEngine->XRSystem->GetHMDDevice()->IsHMDEnabled();
}

FVector AVR_Character_Parent::GetBottomOfCharacter_Implementation()
{
	return FVector(VRRoot->GetComponentLocation());
}

//Hands
void AVR_Character_Parent::Server_SpawnHands_Implementation()
{
	SpawnHands();
}

void AVR_Character_Parent::SpawnHands_Implementation()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = true ? ESpawnActorCollisionHandlingMethod::AlwaysSpawn : ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	LeftHand = GetWorld()->SpawnActor<AVR_Hand_Parent>(VR_Hand_Left, LeftHandRoot->GetComponentLocation(), LeftHandRoot->GetComponentRotation(), SpawnParams);
	LeftHand->Multicast_AttachHands_Implementation(LeftHand, LeftHandRoot, EAttachmentRule::SnapToTarget);
	
	RightHand = GetWorld()->SpawnActor<AVR_Hand_Parent>(VR_Hand_Right, RightHandRoot->GetComponentLocation(), RightHandRoot->GetComponentRotation(), SpawnParams);
	RightHand->Multicast_AttachHands_Implementation(RightHand, RightHandRoot, EAttachmentRule::SnapToTarget);
}


