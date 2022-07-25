#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/InteractInterface.h"
#include "PickUpComp.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class GERZEL_VR_API UPickUpComp : public UActorComponent, public IInteractInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPickUpComp();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR | Interaction")
	void PickUpItem(UPrimitiveComponent* AttachToThis);

	virtual void PickUpItem_Implementation(UPrimitiveComponent* AttachToThis);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "VR | Interaction")
	void Server_PickUpItem(UPrimitiveComponent* AttachToThis);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "VR | Interaction")
	void Multicast_PickUpItem(UPrimitiveComponent* AttachToThis);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR | Interaction")
	void DropItem();

	virtual void DropItem_Implementation();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "VR | Interaction")
	void Server_DropItem();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "VR | Interaction")
	void Multicast_DropItem();

public:
	void PickUp_Implementation(UPrimitiveComponent* AttachToThis) override;
	
	void Drop_Implementation() override;
};
