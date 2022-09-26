// Fill out your copyright notice in the Description page of Project Settings.


#include "SceneComponents/TeleportComp.h"

#include <ThirdParty/Vulkan/Include/vulkan/vulkan_core.h>

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"
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

	TeleportLogic();
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
	if(!PawnToTeleport) return;
	
	if(CollectionParamValid())
	{
		if(!UKismetMaterialLibrary::GetScalarParameterValue(this, MaterialCollection, ParamName) == 0) return;
		
		UKismetMaterialLibrary::SetScalarParameterValue(this, MaterialCollection, ParamName, 1.0f);
	}

	Server_BeginTeleport(true, PawnToTeleport);
}

void UTeleportComp::TryToTeleport_Implementation()
{
	if(CollectionParamValid())
	{
		if(!UKismetMaterialLibrary::GetScalarParameterValue(this, MaterialCollection, ParamName) == 1 || !bTryingToTele) return;
		
		UKismetMaterialLibrary::SetScalarParameterValue(this, MaterialCollection, ParamName, 0.0f);
	}
	if(bTryingToTele)
	{
		Server_TeleportPawn();
	}
}

void UTeleportComp::TeleportPawn_Implementation()
{
	if(bTryingToTele)
	{
		HideTeleportVis();
		
		if(bValidTeleportLocation && Pawn)
		{
			Pawn->SetActorLocation(TraceEnd + TeleportOffsetLocation);
			Server_BeginTeleport(false, nullptr);
		}
		else
		{
			Server_BeginTeleport(false, nullptr);
		}
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

void UTeleportComp::TraceResult_Implementation(FHitResult Hit)
{
	TraceStart = Hit.TraceStart;

	if(Hit.IsValidBlockingHit())
	{
		
	}
	else
	{
		
	}
}

FHitResult UTeleportComp::LineTrace(USceneComponent* SceneComp, FVector Offset, float TraceDist)
{
	FHitResult Hit;
	
	if(SceneComp)
	{
		FVector Start = SceneComp->GetComponentLocation()+Offset;
		FVector End = (SceneComp->GetForwardVector()* TraceDistance) + Start;
		
		GetWorld()->LineTraceSingleByChannel(Hit, Start, End,ECC_Visibility);

		return Hit;
	}
	return Hit;
}

bool UTeleportComp::CollectionParamValid()
{
	return (MaterialCollection && ParamName.IsValid());
}

void UTeleportComp::TeleportLogic()
{
	if(!bTryingToTele) return;

	if(Pawn->IsLocallyControlled())
	{
		TraceResult(LineTrace(this, FVector(0.0f, 0.0f,0.0f), TraceDistance));
	}
	else
	{
		TeleportVis();
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



