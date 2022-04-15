// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/Parent/VR_Character_Parent.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "MotionControllerComponent.h"
#include "Hands/VR_Hand_Parent/VR_Hands_Parent.h"
#include "Components/CapsuleComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"
#include "Teleport/Parent/TeleportActor_Parent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


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

	TeleportMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TeleportMesh"));
	TeleportMesh->SetupAttachment(GetRootComponent());
	TeleportMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	
	LeftHandRoot = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftHandRoot"));
	LeftHandRoot->SetupAttachment(VRRoot);
	LeftHandRoot->MotionSource = FName(TEXT("Left"));
	RightHandRoot = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightHandRoot"));
	RightHandRoot->SetupAttachment(VRRoot);
	RightHandRoot->MotionSource = FName(TEXT("Right"));
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
	SavePlayerController();
}

// Called every frame
void AVR_Character_Parent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CameraOffset();
	ScanForTeleportLocation();
}

// Called to bind functionality to input
void AVR_Character_Parent::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("TeleportLeft"), IE_Pressed, this, &AVR_Character_Parent::TeleportLeftHand);
	PlayerInputComponent->BindAction(TEXT("TeleportLeft"), IE_Released, this, &AVR_Character_Parent::StopTryingToTeleport);

}

// Setup Replicated variables
void AVR_Character_Parent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AVR_Character_Parent, bTryingToTeleport);
	DOREPLIFETIME(AVR_Character_Parent, TraceFromHere);
	DOREPLIFETIME(AVR_Character_Parent, TeleportLocation);
	DOREPLIFETIME(AVR_Character_Parent, bValidTeleportLocation);
}



void AVR_Character_Parent::CameraOffset_Implementation()
{
	FVector NewCameraOffset = Camera->GetComponentLocation() - GetActorLocation();
	NewCameraOffset.Z = 0;
	AddActorWorldOffset(NewCameraOffset);
	VRRoot->AddWorldOffset(-NewCameraOffset);
}

void AVR_Character_Parent::SetCameraToFloor_Implementation()
{
	float ActorHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FVector CameraOffset = VRRoot->GetRelativeLocation();
	CameraOffset.Z = -ActorHalfHeight;
	VRRoot->SetRelativeLocation(CameraOffset);
}

void AVR_Character_Parent::ScanToTeleport_Implementation(USceneComponent* TraceLineFromHere)
{
	if (!bUseVRTeleport || bTryingToTeleport) return;

	if (TraceLineFromHere == nullptr)
	{
		TraceLineFromHere = Camera;
	}

	float ActorHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FVector BottomOfActor = GetActorLocation();
	BottomOfActor.Z = -ActorHalfHeight;

	TeleportLocation = BottomOfActor;

	Server_ScanToTeleport(TraceLineFromHere);
}

void AVR_Character_Parent::Server_ScanToTeleport_Implementation(USceneComponent* TraceLineFromHere)
{
	if (!bUseVRTeleport || bTryingToTeleport || TraceLineFromHere == nullptr) return;

	bTryingToTeleport = true;

	TraceFromHere = TraceLineFromHere;
}

void AVR_Character_Parent::ScanForTeleportLocation_Implementation()
{
	{
		if (!bUseVRTeleport || !bTryingToTeleport || TraceFromHere == nullptr) return;

		if (!HasAuthority()) return;


		FHitResult HitResult;
		FVector StartLocation = TraceFromHere->GetComponentLocation() + TeleportOffset;
		FVector EndLocation = StartLocation + TraceFromHere->GetForwardVector() * MaxTeleportRange;

		bool bLineTraceHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility);

		if (!bLineTraceHit)
		{
			if (bDebugTeleportation)
			{
				DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 0.0f, 1.0f);
			}
			bValidTeleportLocation = false;
			return;
		}

		if (Cast<ATeleportActor_Parent>(HitResult.Actor))
		{
			if (bDebugTeleportation)
			{
				DrawDebugLine(GetWorld(), StartLocation, HitResult.Location, FColor::Blue, false, 0.0f, 1.0f);
				DrawDebugBox(GetWorld(), HitResult.Location, FVector(5.0f, 5.0f, 5.0f), FColor::Blue, false, 0.0f, 10.0f);
			}
			bValidTeleportLocation = true;
			TeleportLocation = HitResult.Location;
			return;
		}


		FNavLocation NavLocation;
		bool bOnNavMesh = UNavigationSystemV1::GetNavigationSystem(GetWorld())->ProjectPointToNavigation(HitResult.Location, NavLocation, TeleportProjectionExtent);

		if (!bOnNavMesh)
		{
			DrawDebugLine(GetWorld(), StartLocation, HitResult.Location, FColor::Red, false, 0.0f, 1.0f);
			DrawDebugBox(GetWorld(), HitResult.Location, FVector(5.0f, 5.0f, 5.0f), FColor::Red, false, 0.0f, 10.0f);
			bValidTeleportLocation = false;
			return;
		}

		bValidTeleportLocation = true;
		TeleportLocation = HitResult.Location;

		if (bDebugTeleportation)
		{
			DrawDebugLine(GetWorld(), StartLocation, HitResult.Location, FColor::Green, false, 0.0f, 1.0f);
			DrawDebugBox(GetWorld(), HitResult.Location, FVector(5.0f, 5.0f, 5.0f), FColor::Green, false, 0.0f, 10.0f);
		}
	}
}

void AVR_Character_Parent::TeleportLeftHand()
{
	ScanToTeleport(LeftHandRoot);
}

void AVR_Character_Parent::StopTryingToTeleport_Implementation()
{
	if (bValidTeleportLocation && bTryingToTeleport)
	{
		if (OwningPlayerController != nullptr)
		{
			OwningPlayerController->PlayerCameraManager->StartCameraFade(0, 1, TeleportFadeTime, FLinearColor::Black, true, true);
		}

		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, this, &AVR_Character_Parent::TeleportUser, TeleportFadeTime, false);
	}
	else
	{
		Server_TeleportUser();
	}
}

void AVR_Character_Parent::TeleportUser_Implementation()
{
	if (bTryingToTeleport && bValidTeleportLocation)
	{
		if (OwningPlayerController != nullptr)
		{
			OwningPlayerController->PlayerCameraManager->StartCameraFade(1, 0, TeleportFadeTime, FLinearColor::Black, true, true);
		}
		Server_TeleportUser();
	}
}

void AVR_Character_Parent::Server_TeleportUser_Implementation()
{
	if (HasAuthority())
	{
		if (bTryingToTeleport && bValidTeleportLocation)
		{
			float ActorHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
			TeleportLocation.Z = +ActorHalfHeight;

			SetActorLocation(TeleportLocation);
		}
		else
		{

		}

		bTryingToTeleport = false;
	}
}

void AVR_Character_Parent::SavePlayerController_Implementation()
{
	OwningPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
}


