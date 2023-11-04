#pragma once

#include "CoreMinimal.h"
#include "XRInteractionComponent.h"
#include "XRInteractionGrab.generated.h"

class UXRReplicatedPhysicsComponent;

UCLASS(ClassGroup = (XRToolkit), meta = (BlueprintSpawnableComponent))
class XR_TOOLKIT_API UXRInteractionGrab : public UXRInteractionComponent
{
	GENERATED_BODY()

public:
    UXRInteractionGrab();
    virtual void BeginPlay() override;

    void StartInteraction(UXRInteractorComponent* InInteractor) override;
    void EndInteraction(UXRInteractorComponent* InInteractor) override;


    /**
    * Enable Replicated Physics. 
    * Actor MUST have a UStaticMeshComponent as the RootComponent. 
    */
    UPROPERTY(EditAnywhere, Category = "XRToolkit|XR Interaction|Config")
    bool bEnablePhysics = true;
    /**
    * Optionally specify additional components in the actor with this tag that should be Physics enabled and replicated.
    * Actor MUST have a UStaticMeshComponent as the RootComponent (this is cached even without the tag).
    */
    UPROPERTY(EditAnywhere, Category = "XRToolkit|XR Interaction|Config")
    FName PhysicsTag = "XRPhysics";

    /**
    * Return the Component handling PhysicsReplication for this Interaction. If bEnablePhysics is true, this component will be spawned at BeginPlay().
    */
    UFUNCTION(BlueprintPure, Category = "XRToolkit|XR Interaction|Config")
    UXRReplicatedPhysicsComponent* GetPhysicsReplicationComponent();

protected:
    UPROPERTY()
    UXRReplicatedPhysicsComponent* XRReplicatedPhysicsComponent = nullptr;

    UFUNCTION()
    void AttachOwningActorToXRInteractor(UXRInteractorComponent* InInteractor);

    UFUNCTION()
    void DetachOwningActorFromXRInteractor();

    UFUNCTION()
    void PhysicsGrab(UXRInteractorComponent* InInteractor);
    UFUNCTION()
    void PhysicsUngrab(UXRInteractorComponent* InInteractor);

private:
    UFUNCTION()
    void InitializePhysics();
};