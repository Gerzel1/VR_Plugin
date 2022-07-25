// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VR_Hand_Parent.generated.h"

UCLASS()
class GERZEL_VR_API AVR_Hand_Parent : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVR_Hand_Parent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "VR | Hands")
	void Multicast_AttachHands(AActor* Actor, USceneComponent* SceneComponent, EAttachmentRule AttachRule);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR | Hands")
	void AttachHands(AActor* Actor, USceneComponent* SceneComponent, EAttachmentRule AttachRule);

	virtual void AttachHands_Implementation(AActor* Actor, USceneComponent* SceneComponent, EAttachmentRule AttachRule);
};
