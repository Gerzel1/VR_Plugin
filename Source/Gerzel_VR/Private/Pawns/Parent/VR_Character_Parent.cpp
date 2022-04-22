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

	BeamStart = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Beam Start"));
	BeamStart->SetupAttachment(VRRoot);
	BeamStart->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BeamStart->SetHiddenInGame(true);

	Beam = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Beam"));
	Beam->SetupAttachment(BeamStart);
	Beam->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Beam->SetHiddenInGame(true);

	BeamEnd = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Beam End"));
	BeamEnd->SetupAttachment(BeamStart);
	BeamEnd->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BeamEnd->SetHiddenInGame(true);  
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
	SpawnTeleportIcon();
	SpawnTeleportBeam();
}

FVector AVR_Character_Parent::GetBottomOfCharacter_Implementation()
{
	float ActorHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FVector Offset = GetActorLocation();
	Offset.Z = -ActorHalfHeight;
	return FVector(Offset);
}

// Called every frame
void AVR_Character_Parent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CameraOffset();
	ScanForTeleportLocation();
	DrawTeleportLine();
}

// Called to bind functionality to input
void AVR_Character_Parent::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("TeleportLeft"), IE_Pressed, this, &AVR_Character_Parent::TeleportLeftHand);
	PlayerInputComponent->BindAction(TEXT("TeleportLeft"), IE_Released, this, &AVR_Character_Parent::StopTryingToTeleportLeft);

}

// Setup Replicated variables
void AVR_Character_Parent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AVR_Character_Parent, bTryingToTeleport);
	DOREPLIFETIME(AVR_Character_Parent, TraceFromHere);
	DOREPLIFETIME(AVR_Character_Parent, TeleportLocation);
	DOREPLIFETIME(AVR_Character_Parent, bValidTeleportLocation);
	DOREPLIFETIME(AVR_Character_Parent, TeleportEnd);
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

		if (HasAuthority())
		{
			FHitResult HitResult;
			FVector StartLocation = TraceFromHere->GetComponentLocation() + TeleportOffset;
			FVector EndLocation = StartLocation + TraceFromHere->GetForwardVector() * MaxTeleportRange;

			bool bLineTraceHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility);

			if (!bLineTraceHit)
			{
				TeleportEnd = HitResult.TraceEnd;
				bValidTeleportLocation = false;
				return;
			}

			if (Cast<ATeleportActor_Parent>(HitResult.Actor))
			{
				TeleportLocation = HitResult.Location;
				TeleportEnd = TeleportLocation;
				bValidTeleportLocation = true;
				return;
			}

			FNavLocation NavLocation;
			bool bOnNavMesh = UNavigationSystemV1::GetNavigationSystem(GetWorld())->ProjectPointToNavigation(HitResult.Location, NavLocation, TeleportProjectionExtent);

			if (!bOnNavMesh)
			{
				TeleportEnd = HitResult.Location;
				bValidTeleportLocation = false;
				return;
			}

			TeleportLocation = HitResult.Location;
			TeleportEnd = TeleportLocation;
			bValidTeleportLocation = true;
			return;
		}
	}
}

void AVR_Character_Parent::DrawTeleportLine_Implementation()
{
	if (bTryingToTeleport)
	{
		if (bDebugTeleportation)
		{
			if (bValidTeleportLocation)
			{
				DrawDebugLine(GetWorld(), TraceFromHere->GetComponentLocation() + TeleportOffset, TeleportEnd, FColor::Green, false, 0.0f, 1.0f);
				DrawDebugBox(GetWorld(), TeleportEnd, FVector(5.0f, 5.0f, 5.0f), FColor::Blue, false, 0.0f, 10.0f);
			}
			else
			{
				DrawDebugLine(GetWorld(), TraceFromHere->GetComponentLocation() + TeleportOffset, TeleportEnd, FColor::Red, false, 0.0f, 1.0f);
			}
		}
		else
		{
			if (TeleportLocationIcon)
			{
				TeleportLocationIcon->SetActorHiddenInGame(false);

				FVector NewLocation = FMath::VInterpTo(TeleportLocationIcon->GetActorLocation(), TeleportLocation, GetWorld()->DeltaTimeSeconds, 5.0f);

				TeleportLocationIcon->SetActorLocation(NewLocation);
				TeleportLocationIcon->SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));

				if (Beam && BeamStart && BeamEnd)
				{
					float Length = FVector::Dist(TraceFromHere->GetComponentLocation() + TeleportOffset, TeleportEnd);
					BeamStart->SetWorldLocation(TraceFromHere->GetComponentLocation() + TeleportOffset);
					BeamStart->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(TraceFromHere->GetComponentLocation() + TeleportOffset, TeleportEnd));

					Beam->SetRelativeScale3D(FVector(Length, 1.0f, 1.0f));

					BeamEnd->SetWorldLocation(TeleportEnd);

					Beam->SetHiddenInGame(false);
					BeamStart->SetHiddenInGame(false);
					BeamEnd->SetHiddenInGame(false);
				}
			}
			else
			{
				SpawnTeleportIcon();
			}
		}
	}
	else
	{
		if (TeleportLocationIcon)
		{
			TeleportLocationIcon->SetActorHiddenInGame(true);
			Beam->SetHiddenInGame(true);
			BeamStart->SetHiddenInGame(true);
			BeamEnd->SetHiddenInGame(true);
			Beam->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
		}
	}
}

void AVR_Character_Parent::TeleportLeftHand_Implementation()
{
	if (bTryingToTeleport) return;

	bTeleportLeftHand = true;
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

void AVR_Character_Parent::StopTryingToTeleportLeft_Implementation()
{
	if (bTryingToTeleport && bTeleportLeftHand)
	{
		StopTryingToTeleport();
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

void AVR_Character_Parent::SpawnTeleportIcon_Implementation()
{
	if (TeleportLocationIcon) return;


	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = true ? ESpawnActorCollisionHandlingMethod::AlwaysSpawn : ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	TeleportLocationIcon = GetWorld()->SpawnActor<ATeleportLocationIcon_Parent>(TeleportLocationIconClass, GetBottomOfCharacter(), GetActorRotation(), SpawnParams);
	TeleportLocationIcon->SetActorHiddenInGame(true);

	TeleportLocationIcon->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
}

void AVR_Character_Parent::SpawnTeleportBeam_Implementation()
{
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


