
#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "InputCoreTypes.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "XRInteractorComponent.generated.h"

class UXRInteractionComponent;
class UXRInteractorComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStartedInteracting, UXRInteractorComponent*, Sender, UXRInteractionComponent*, XRInteractionComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStoppedInteracting, UXRInteractorComponent*, Sender, UXRInteractionComponent*, XRInteractionComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHoverStateChanged, UXRInteractorComponent*, Sender, UXRInteractionComponent*, HoveredXRInteractionComponent, bool, bHoverState);

UCLASS( ClassGroup=(XRToolkit), meta=(BlueprintSpawnableComponent) )
class XR_TOOLKIT_API UXRInteractorComponent : public USphereComponent
{
	GENERATED_BODY()

public:	
	UXRInteractorComponent();

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Interaction Events
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------

	/**
	 * Will start an Interaction (Replicated).
	 * If the InteractionComponent is already interacted with, the current interaction will be terminated first.
	 * Note: This is intended for manual interaction handling - consider using AutoStartXRInteraction().
	 */
	UFUNCTION(BlueprintCallable, Category = "XRCore|Interactor")
	void StartXRInteraction(UXRInteractionComponent* InInteractionComponent = nullptr);

	/**
	 * Start interacting with the next available XRInteractionComponent. 
	 * Will interact with the most prioritized interaction on the closest found actor with an XRInteractionComponent OR the most prioritized interaction on the actor 
	 * that is already being interacted with (using multiple interactions on one Actor).
	 */
	UFUNCTION(BlueprintCallable, Category = "XRCore|Interactor")
	void AutoStartXRInteraction();

	UPROPERTY(BlueprintAssignable, Category = "XRCore|Interactor|Delegates")
	FOnStartedInteracting OnStartedInteracting;

	/**
	 * Will stop an Interaction. (Replicated).
	 * If the InteractionComponent is not interacting, the command is discarded.
	 * Note: This is intended for manual interaction handling - consider using AutoStopXRInteraction().
	 */
	UFUNCTION(BlueprintCallable, Category = "XRCore|Interactor")
	void StopXRInteraction(UXRInteractionComponent* InXRInteraction = nullptr);

	/**
	 * Will stop interacting with the least prioritized interaction on the closest found actor with an XRInteractionComponent OR the least prioritized interaction on the actor 
	 * that is already being interacted with (using multiple interactions on one Actor). 
	 */
	UFUNCTION(BlueprintCallable, Category = "XRCore|Interactor")
	void AutoStopXRInteraction();

	/**
	 * Terminate all active Interactions.
	 */
	UFUNCTION(BlueprintCallable, Category = "XRCore|Interactor")
	void StopAllXRInteractions();

	UPROPERTY(BlueprintAssignable, Category = "XRCore|Interactor|Delegates")
	FOnStoppedInteracting OnStoppedInteracting;
	
	UPROPERTY(BlueprintAssignable, Category = "XRCore|Interactor|Delegates")
	FOnHoverStateChanged OnHoverStateChanged;

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Utility
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	/**
	 * Returns if any InteractionComponent is available in proximity to this XRInteractor. Also returns the available XRInteractionComponent (prioritized).
	 */
	UFUNCTION(BlueprintPure, Category = "XRCore|Interactor")
	bool CanInteract(UXRInteractionComponent*& OutPrioritizedXRInteraction) const;

	/**
	 * Returns true if a valid InteractionComponent is assigned. 
	 */
	UFUNCTION(BlueprintPure, Category="XRCore|Interactor")
	bool IsInteracting() const;
	/**
	 * Returns all active Interactions (iE. Continuous Interactions this Interactor is handling)
	 */
	UFUNCTION(BlueprintPure, Category="XRCore|Interactor")
	TArray<UXRInteractionComponent*> GetActiveInteractions() const; 

	/**
	 * Returns XRInteractionComponents on the closest overlapping Actor (that has XRInteractions) for this XRInteractor.
	 * Distance is calculated based on the Actors root, not the contained XRInteractionComponents location.
	 */
	UFUNCTION(BlueprintPure, Category="XRCore|Interactor")
	AActor* GetClosestXRInteractionActor() const;


	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Config
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	/**
	 * Set the HandType for this Interactor.
	*/
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "XRCore|Interactor")
	void Server_SetXRControllerHand(EControllerHand InXRControllerHand);
	/**
	 * Get the assigned HandType for this Interactor.
	*/
	UFUNCTION(BlueprintPure, Category = "XRCore|Interactor")
	EControllerHand GetXRControllerHand() const;

	/**
	 * Returns true if this Interactor is part of an XRLaser. This can be useful to distinguish between Interactions
	 * coming from Hands or Lasers to allow for different behavior. (For example SnapToHand-grab via Laser) 
	*/
	UFUNCTION(BlueprintPure, Category="XRCore|Interactor")
	bool IsLaserInteractor();

	/**
	 * Manually set the assochiated Pawn - this is useful for Actors that need a XRInteractor but are not an APawn like the XRLaser.
	 * Is done automatically at BeginPlay if this XRInteractor is owned by an APawn.
	*/
	UFUNCTION(BlueprintCallable, Category = "XRCore|Interactor")
	void SetOwningPawn(APawn* InOwningPawn);
	/**
	 * Return the assochiated Pawn. Useful for intermediaries that use XRInteractors like the XRLaser.
	*/
	UFUNCTION(BlueprintPure, Category = "XRCore|Interactor")
	APawn* GetOwningPawn() const;
	/**
	 * Is the Owner (Pawn) of this XRInteractor locally controlled?
	*/
	UFUNCTION(BlueprintPure, Category="XRCore|Interactor")
	bool IsLocallyControlled() const;

	/**
	 * Required Physics-based Interactions. Not spawned automatically as manual assignment gives greater flexibility in configuration of the PhysicsConstraint.
	 * @param InPhysicsConstraintComponent PhysicsConstraintComponent to assign to this XRInteractor.
	*/
	UFUNCTION(BlueprintCallable, Category = "XRCore|Interactor")
	void SetPhysicsConstraint(UPhysicsConstraintComponent* InPhysicsConstraintComponent);

	/**
	 * Set the Colliders which this Interactor will listen to for Overlap events. Will override and unbind from pre-existing colliders.
	 * Also used for AutoStartXRInteraction and AutoStopXRInteraction methods.
	*/
	UFUNCTION(BlueprintCallable, Category = "XRCore|Interactor")
	void SetAdditionalColliders(TArray<UPrimitiveComponent*> InColliders);

	/**
	 * Get the Colliders which this Interactor listens to for Overlap events.
	*/
	UFUNCTION(BlueprintPure, Category = "XRCore|Interactor")
	TArray<UPrimitiveComponent*> GetAdditionalColliders() const;

	UFUNCTION(BlueprintCallable, Category = "XRCore|Interactor")
	TArray<AActor*> GetAllOverlappingActors() const;

	/**
	 * Return the associated PhysicsConstraint.
	 * NOTE: Must be assigned manually first. 
	*/
	UFUNCTION(BlueprintPure, Category="XRCore|Interactor")
	UPhysicsConstraintComponent* GetPhysicsConstraint() const;
	

	
protected:
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;


	UPROPERTY()
	TArray<UPrimitiveComponent*> AdditionalColliders = {};

	UPROPERTY(Replicated, EditDefaultsOnly, Category="XRCore|Interactor")
	EControllerHand XRControllerHand = EControllerHand::AnyHand;
	
	UPROPERTY(EditDefaultsOnly, Category="XRCore|Interactor")
	bool bIsLaserInteractor = false;

	UFUNCTION()
	void HoverActor(AActor* OtherActor, bool bHoverState);

	UFUNCTION(Server, Reliable, Category = "XRCore|Interactor")
	void Server_ExecuteInteraction(UXRInteractionComponent* InInteractionComponent);
	UFUNCTION(NetMulticast, Reliable, Category = "XRCore|Interactor")
	void Multicast_ExecuteInteraction(UXRInteractionComponent* InteractionComponent);


	UFUNCTION(Server, Reliable, Category = "XRCore|Interactor")
	void Server_TerminateInteraction(UXRInteractionComponent* InInteractionComponent);
	UFUNCTION(NetMulticast, Reliable, Category = "XRCore|Interactor")
	void Multicast_TerminateInteraction(UXRInteractionComponent* InteractionComponent);
	
private:
	UPROPERTY()
	APawn* OwningPawn = nullptr;
	UPROPERTY()
	UPhysicsConstraintComponent* PhysicsConstraint;
	UPROPERTY()
	AActor* LocalInteractedActor = nullptr;
	UPROPERTY()
	TArray<UXRInteractionComponent*> LocalHoveredInteractions = {};
	UPROPERTY(Replicated)
	TArray<UXRInteractionComponent*> ActiveInteractionComponents = {};
	UPROPERTY()
	TArray<UXRInteractionComponent*> HoveredInteractionComponents = {};

	void CacheIsLocallyControlled();
	bool bIsLocallyControlled = false;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};