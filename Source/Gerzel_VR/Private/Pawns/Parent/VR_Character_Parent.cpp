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
#include "Teleport/Parent/TeleportLocationIcon_Parent.h"
#include "Math/UnrealMathUtility.h"
#include "Math/Vector.h"
#include "Kismet/KismetMathLibrary.h"
#include "BeamEffects/Parent/Beam_Parent.h"
#include "IXRTrackingSystem.h"
#include "IHeadMountedDisplay.h"


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

	SpawnTeleportBeam();
	SpawnTeleportIcon();
}

// Called every frame
void AVR_Character_Parent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Server_TeleportLogic();
	TeleportVisual();
}

// Called to bind functionality to input
void AVR_Character_Parent::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//PlayerInputComponent->BindAction(TEXT("TeleportLeft"), IE_Pressed, this, &AVR_Character_Parent::);
	//PlayerInputComponent->BindAction(TEXT("TeleportLeft"), IE_Released, this, &AVR_Character_Parent::);

	//PlayerInputComponent->BindAction(TEXT("TeleportRight"), IE_Pressed, this, &AVR_Character_Parent::);
	//PlayerInputComponent->BindAction(TEXT("TeleportRight"), IE_Released, this, &AVR_Character_Parent::);

}

// Setup Replicated variables
void AVR_Character_Parent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AVR_Character_Parent, bTryingToTeleport);
	DOREPLIFETIME(AVR_Character_Parent, TraceFromHere);
	DOREPLIFETIME(AVR_Character_Parent, TeleportLocation);
	DOREPLIFETIME(AVR_Character_Parent, TeleportEnd);
	DOREPLIFETIME(AVR_Character_Parent, bValidTeleportLocation);
	DOREPLIFETIME(AVR_Character_Parent, TeleportLocationIconClass);
	DOREPLIFETIME(AVR_Character_Parent, TeleportBeamClass);
	DOREPLIFETIME(AVR_Character_Parent, VR_Hand_Left);
	DOREPLIFETIME(AVR_Character_Parent, VR_Hand_Right);
	DOREPLIFETIME_CONDITION(AVR_Character_Parent, HeadTransform, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AVR_Character_Parent, LeftHandTransform, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AVR_Character_Parent, RightHandTransform, COND_SkipOwner);
}


//Camera
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

//Teleportation
void AVR_Character_Parent::SpawnTeleportIcon_Implementation()
{
	if (TeleportLocationIcon) return;

	if (!TeleportLocationIconClass)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("VR Character - 'TeleportLocationIconClass' is invalid"));
		return;
	}


	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = true ? ESpawnActorCollisionHandlingMethod::AlwaysSpawn : ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	TeleportLocationIcon = GetWorld()->SpawnActor<ATeleportLocationIcon_Parent>(TeleportLocationIconClass, GetBottomOfCharacter(), GetActorRotation(), SpawnParams);
	TeleportLocationIcon->SetActorHiddenInGame(true);
}

void AVR_Character_Parent::SpawnTeleportBeam_Implementation()
{
	if (TeleportBeam) return;

	if (!TeleportBeamClass)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("VR Character - 'TeleportBeamClass' is invalid"));
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = true ? ESpawnActorCollisionHandlingMethod::AlwaysSpawn : ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	TeleportBeam = GetWorld()->SpawnActor<ABeam_Parent>(TeleportBeamClass, GetBottomOfCharacter(), GetActorRotation(), SpawnParams);
	TeleportBeam->SetActorHiddenInGame(true);
}

void AVR_Character_Parent::TeleportLogic_Implementation()
{
	if (bTryingToTeleport && HasAuthority() && TraceFromHere && !bIsTeleporting)
	{
		FHitResult HitResult;
		FVector Start = TraceFromHere->GetComponentLocation() + TeleportOffset;
		FVector End = TraceFromHere->GetComponentLocation() + TraceFromHere->GetForwardVector() * MaxTeleportRange;

		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility);

		if (!bHit)
		{
			TeleportEnd = HitResult.TraceEnd;
			bValidTeleportLocation = false;
			return;
		}

		if (Cast<ATeleportActor_Parent>(HitResult.Actor))
		{
			TeleportEnd = HitResult.Location;
			TeleportLocation = TeleportEnd;
			bValidTeleportLocation = true;
			//TODO add functionality for different teleport types
			return;
		}
		else
		{
			FNavLocation NavLocation;
			bool bOnNavMesh = UNavigationSystemV1::GetNavigationSystem(GetWorld())->ProjectPointToNavigation(HitResult.Location, NavLocation, TeleportProjectionExtent);

			if (bOnNavMesh)
			{
				TeleportEnd = HitResult.Location;
				TeleportLocation = TeleportEnd;
				bValidTeleportLocation = true;
				return;
			}
			else
			{
				TeleportEnd = HitResult.Location;
				bValidTeleportLocation = false;
				return;
			}
		}
	}
}

void AVR_Character_Parent::TeleportVisual_Implementation()
{
	if (bTryingToTeleport && TraceFromHere && !bIsTeleporting)
	{
		if (TeleportBeam)
		{
			if (TraceFromHere)
			{
				TeleportBeam->DrawBeam(TraceFromHere->GetComponentLocation() + TeleportOffset, TeleportEnd, bValidTeleportLocation);
				TeleportBeam->SetActorHiddenInGame(false);
			}
		}

		if (TeleportLocationIcon)
		{
			TeleportLocationIcon->MoveActor(TeleportLocation, TeleportEnd, bValidTeleportLocation);
			TeleportLocationIcon->SetActorHiddenInGame(false);
		}
	}
	
	if (!bTryingToTeleport)
	{
		
		if (TeleportBeam)
		{
			TeleportBeam->SetActorHiddenInGame(true);
		}

		if (TeleportLocationIcon)
		{
			TeleportLocationIcon->SetActorHiddenInGame(true);
		}
		

		if (TeleportBeam)
		{
			TeleportBeam->SetActorLocation(GetBottomOfCharacter());
		}

		if (TeleportLocationIcon)
		{
			TeleportLocationIcon->SetActorLocation(GetBottomOfCharacter());
		}
	}
}

void AVR_Character_Parent::TeleportUser_Implementation()
{
	if (bTryingToTeleport && !bIsTeleporting)
	{
		if (bValidTeleportLocation)
		{
			bIsTeleporting = true;
			SetActorLocation(TeleportLocation + FVector(0.0f, 0.0f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
			TeleportLocation = GetBottomOfCharacter();
			TraceFromHere = nullptr;
			bValidTeleportLocation = false;
			bTryingToTeleport = false;
			//TODO add teleporting effect
			bIsTeleporting = false;
		}
		else
		{
			TeleportLocation = GetBottomOfCharacter();
			TraceFromHere = nullptr;
			bValidTeleportLocation = false;
			bTryingToTeleport = false;
		}

		
		if (TeleportBeam)
		{
			TeleportBeam->SetActorHiddenInGame(true);
		}

		if (TeleportLocationIcon)
		{
			TeleportLocationIcon->SetActorHiddenInGame(true);
		}
		
	}
}

void AVR_Character_Parent::StartTeleport_Implementation(USceneComponent* TraceFromComponent)
{
	//if (bIsTeleporting) return;

	if (!TraceFromComponent)
	{
		if (TeleportBeam)
		{
			TeleportBeam->SetActorLocation(GetBottomOfCharacter());
		}

		if (TeleportLocationIcon)
		{
			TeleportLocationIcon->SetActorLocation(GetBottomOfCharacter());
		}

		if (Camera)
		{
			TraceFromHere = Camera;
			bTryingToTeleport = true;
			return;
		}
		else
		{
			bTryingToTeleport = false;
			return;
		}
	}
	else
	{
		TraceFromHere = TraceFromComponent;
		bTryingToTeleport = true;
	}
}


//Teleport Server
void AVR_Character_Parent::Server_StartTeleport_Implementation(USceneComponent* TraceFromComponent)
{
	StartTeleport(TraceFromComponent);
}

void AVR_Character_Parent::Server_TeleportLogic_Implementation()
{
	TeleportLogic();
}

void AVR_Character_Parent::Server_TeleportUser_Implementation()
{
	TeleportUser();
}


//Character
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

void AVR_Character_Parent::Client_UpdateHandsAndHead_Implementation()
{
	if (IsLocallyControlled())
	{
		HeadTransform = Camera->GetComponentTransform();
		LeftHandTransform = LeftHandRoot->GetComponentTransform();
		RightHandTransform = RightHandRoot->GetComponentTransform();

		Server_UpdateHandsAndHead(HeadTransform, LeftHandTransform, RightHandTransform);
	}
	else
	{
		Camera->SetWorldTransform(HeadTransform);
		LeftHandRoot->SetWorldTransform(LeftHandTransform);
		RightHandRoot->SetWorldTransform(RightHandTransform);
	}
}

FVector AVR_Character_Parent::GetBottomOfCharacter_Implementation()
{
	return FVector(VRRoot->GetComponentLocation());
}


