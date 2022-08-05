// Fill out your copyright notice in the Description page of Project Settings.


#include "SceneComponents/TeleportComp.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UTeleportComp::UTeleportComp()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicated(true);
}


void UTeleportComp::BeginPlay()
{
	Super::BeginPlay();

	SpawnObjects();
}


void UTeleportComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTeleportComp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UTeleportComp, bTryingToTele);
	DOREPLIFETIME(UTeleportComp, Pawn);
	DOREPLIFETIME(UTeleportComp, bValidTeleportLocation);
	DOREPLIFETIME(UTeleportComp, TraceEnd);
	DOREPLIFETIME(UTeleportComp, TraceStart);
}

void UTeleportComp::SpawnObjects_Implementation()
{
	if(UGameplayStatics::GetPlayerPawn(GetWorld(), 0) && UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->IsLocallyControlled())
	{
		SpawnBeamClass();
		SpawnTeleportIcon();
	}
}

void UTeleportComp::SpawnBeamClass_Implementation()
{
	if(TeleportBeamClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		SpawnParams.SpawnCollisionHandlingOverride = true ? ESpawnActorCollisionHandlingMethod::AlwaysSpawn : ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		TeleportBeam = GetWorld()->SpawnActor<ABeam_Parent>(TeleportBeamClass, GetComponentLocation(), GetComponentRotation(), SpawnParams);
	}
}

void UTeleportComp::SpawnTeleportIcon_Implementation()
{
	if(TeleportIconClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		SpawnParams.SpawnCollisionHandlingOverride = true ? ESpawnActorCollisionHandlingMethod::AlwaysSpawn : ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		TeleportIcon = GetWorld()->SpawnActor<ATeleportLocationIcon_Parent>(TeleportIconClass, GetComponentLocation(), GetComponentRotation(), SpawnParams);
	}
}

void UTeleportComp::BeginTeleport_Implementation(APawn* PawnToTeleport)
{
}

void UTeleportComp::TryToTeleport_Implementation()
{
}

void UTeleportComp::TeleportPawn_Implementation()
{
	if(bTryingToTele)
	{
		if(bValidTeleportLocation && Pawn)
		{
			Pawn->SetActorLocation(TraceEnd + TeleportOffsetLocation);
			Server_BeginTeleport(false, nullptr);
		}
		else
		{
			Server_BeginTeleport(false, nullptr);
		}
		
		HideTeleportVis();
	}
}

void UTeleportComp::TeleportVis_Implementation()
{
	if(UGameplayStatics::GetPlayerPawn(GetWorld(), 0) && UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->IsLocallyControlled())
	{
		if(TeleportBeam && TeleportBeam->GetClass()->ImplementsInterface(UTeleportInterface::StaticClass()))
		{
			ITeleportInterface::Execute_TeleportTraceResult(TeleportBeam, TraceStart, TraceEnd, bValidTeleportLocation);
		}
		if(TeleportIcon && TeleportIcon->GetClass()->ImplementsInterface(UTeleportInterface::StaticClass()))
		{
			ITeleportInterface::Execute_TeleportTraceResult(TeleportIcon, TraceEnd, TraceEnd, bValidTeleportLocation);
		}
	}
}

void UTeleportComp::HideTeleportVis_Implementation()
{
	if(TeleportBeam && TeleportBeam->GetClass()->ImplementsInterface(UTeleportInterface::StaticClass()))
	{
		ITeleportInterface::Execute_TeleportResult(TeleportBeam, TraceEnd, bValidTeleportLocation);
	}
	if(TeleportIcon && TeleportIcon->GetClass()->ImplementsInterface(UTeleportInterface::StaticClass()))
	{
		ITeleportInterface::Execute_TeleportResult(TeleportIcon, TraceEnd, bValidTeleportLocation);
	}
}

void UTeleportComp::Server_TeleportPawn_Implementation()
{
	TeleportPawn();
}

void UTeleportComp::Server_BeginTeleport_Implementation(bool const TryingToTeleport, APawn* PawnToMove)
{
	bTryingToTele = TryingToTeleport;
	Pawn = PawnToMove;
}



