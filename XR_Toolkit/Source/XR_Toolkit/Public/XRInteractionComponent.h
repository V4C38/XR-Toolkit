
#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Sound/SoundBase.h"
#include "XRInteractionComponent.generated.h"


class UXRInteractorComponent;
class UXRInteractionComponent;
class UXRInteractionHighlightComponent;



DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractionStarted, UXRInteractionComponent*, Sender, UXRInteractorComponent*, XRInteractorComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractionEnded, UXRInteractionComponent*, Sender, UXRInteractorComponent*, XRInteractorComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInteractionHovered, UXRInteractionComponent*, Sender, UXRInteractorComponent*, HoveringXRInteractor, bool, bHovered);


UCLASS(Blueprintable, ClassGroup=(XRToolkit), meta=(BlueprintSpawnableComponent) )
class XR_TOOLKIT_API UXRInteractionComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UXRInteractionComponent();


	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Interaction Events
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	/**
	 * Will manually call the OnInteractionStart Event which should be overriden by the inheriting class with the interaction behavior.
	 * NOTE: Intended to be automatically called from XRInteractionSystem. 
	 * @param InInteractor Optional. 
	 */
	UFUNCTION(BlueprintCallable, Category="XR Interaction")
	virtual void StartInteraction(UXRInteractorComponent* InInteractor);

	UPROPERTY(BlueprintAssignable, Category="XR Interaction|Delegates")
	FOnInteractionStarted OnInteractionStarted;

	
	/**
	 * Will manually call the OnInteractionStop Event which should be overriden by the inheriting class with the interaction behavior.
	 * NOTE: Intended to be automatically called from XRInteractionSystem. 
	 *  @param InInteractor Optional. 
	 */
	UFUNCTION(BlueprintCallable, Category="XR Interaction")
	virtual void EndInteraction(UXRInteractorComponent* InInteractor);
	
	UPROPERTY(BlueprintAssignable, Category="XR Interaction|Delegates")
	FOnInteractionEnded OnInteractionEnded;


	/**
	 * Will manually call the OnInteractionHovered Event which should be overriden by the inheriting class with the interaction behavior.
	 * NOTE: Intended to be automatically called from XRInteractionSystem.
	 *  @param InInteractor Optional.
	 *  @param bInHoverState Set Hover state to true or false - corresponds to 0.0f or 1.0f in the HighlightState Material Parameter.
	 */
	UFUNCTION(BlueprintCallable, Category="XR Interaction")
	virtual void HoverInteraction(UXRInteractorComponent* InInteractor, bool bInHoverState);

	UPROPERTY(BlueprintAssignable, Category = "XR Interaction|Delegates")
	FOnInteractionHovered OnInteractionHovered;


	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Utility
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	/**
	 * Returns true if this is a continuous interaction and it is currently ongoing. 
	 */
	UFUNCTION(BlueprintPure, Category="XR Interaction")
	bool IsInteractionActive() const;

	/**
	 * Sets the Active Interactor if this is a ContinuousInteraction. (Replicated)
	 * @param InInteractor Set to nullptr if you are unassigning an Interactor.
	 */
	UFUNCTION(BlueprintCallable, Category = "XR Interaction")
	void SetActiveInteractor(UXRInteractorComponent* InInteractor);
	/**
	 * Return associated XRInteractorComponent. Can be nullptr. 
	 */
	UFUNCTION(BlueprintPure, Category="XR Interaction")
	UXRInteractorComponent* GetActiveInteractor();

	/**
	 * Return the Priority value for this XRInteractionComponent.
	 */
	UFUNCTION(BlueprintPure, Category="XR Interaction")
	int32 GetInteractionPriority() const;

	/**
	 * Is the interaction finished instantly or must it be ended manually.
	*/
	UFUNCTION(BlueprintPure, Category = "XR Interaction|Config")
	bool IsContinuousInteraction() const;

	/**
	* Can this Continuous Interaction be taken over by another XRInteractor while it is active?
	* Example: GrabInteraction - an XRInteractor starts grabing even though the grab is already active on another.
	 */
	UFUNCTION(BlueprintCallable, Category = "XR Interaction")
	void SetAllowTakeOver(bool bInAllowTakeOver);
	/**
	* Can this Continuous Interaction be taken over by another XRInteractor while it is active?
	* Example: GrabInteraction - an XRInteractor starts grabing even though the grab is already active on another. 
	 */
	UFUNCTION(BlueprintPure, Category="XR Interaction")
	bool GetAllowTakeOver() const;

	/**
	 * Enables / Disables this Interaction to be triggered via the LaserComponent. Will also disable Highlighting via the Laser. 
	 * The Actor will be ignored during the collision checks in the InteractionSystemComponent. 
	 */
	UFUNCTION(Blueprintpure, Category="XR Interaction|Laser")
	bool IsLaserInteractionEnabled() const;

	/**
	 * Set if this XRInteraction disables the XRLaser while the Interaction is active.
	 */
	UFUNCTION(BlueprintCallable, Category = "XR Interaction|Laser")
	void SetSupressLaserWhenInteracting(bool InSupressLaser);
	/**
	 * Should this XRInteraction disable the XRLaser while the Interaction is active.
	 */
	UFUNCTION(BlueprintPure, Category = "XR Interaction|Laser")
	bool GetSupressLaserWhenInteracting() const;

	/**
	 * Should this XRInteraction force the XRLaser to snap to the owning Actor.
	 */
	UFUNCTION(BlueprintPure, Category = "XR Interaction|Laser")
	bool GetSnapXRLaserToActor() const;
	/**
	 * Set if this XRInteraction forces the XRLaser to snap to the owning Actor.
	 */
	UFUNCTION(BlueprintCallable, Category = "XR Interaction|Laser")
	void SetSnapXRLaserToActor(bool InSnapXRLaserToActor);

	/**
	 * Return the assigned XRInteractionHighlightComponent. Only valid if bEnableHighlighting is true on BeginPlay.
	 */
	UFUNCTION(BlueprintPure, Category = "XR Interaction|Highlight")
	UXRHighlightComponent* GetXRHighlightComponent();

protected:
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;

	UPROPERTY()
	bool bIsInteractionActive = false;

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Abstract Interaction Events 
	// Override in derived classes to implement Interaction specific logic.
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	/**
	 * Override in Child Blueprint classes to implement the interaction behavior
	 * @param InInteractor Optional. Can be nullptr.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="XR Interaction")
	void OnInteractionStart(UXRInteractorComponent* InInteractor);
	/**
	 * Override in Child Blueprint classes to implement the interaction behavior
	 * @param InInteractor Optional. Can be nullptr.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="XR Interaction")
	void OnInteractionEnd(UXRInteractorComponent* InInteractor);
	
	/**
	 * Override in Child Blueprint classes.
	 * Called on Interaction Hover Start and End. 
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="XR Interaction")
	void OnInteractionHover(bool bHovering, UXRInteractorComponent* HoveringXRInteractor);

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Config - General
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	/**
	 * Interactions with a higher priority will be started first. Value must be > 0.
	 * This allows stacking multiple Interactions on one Actor and starting / stopping them independently from each other.
	 * Lower Prio is better: 1 will be prioritized over 2 etc.
	 */
	UPROPERTY(EditAnywhere, Category = "XR Interaction|Config")
	int32 InteractionPriority = 1;

	/**
	 * Determines if the interaction is finished instantly or must be ended manually.
	 * Enables In-Progress Late-Joining.
	 */
	UPROPERTY(EditAnywhere, Category = "XR Interaction|Config")
	bool bIsContinuousInteraction = true;

	/**
	* Can this Continuous Interaction be taken over by another XRInteractor while it is active?
	* Example: GrabInteraction - an XRInteractor starts grabing even though the grab is already active on another.
	*/
	UPROPERTY(EditAnywhere, Category = "XR Interaction|Config")
	bool bAllowTakeOver = true;

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Config - Laser
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	/**
	 * Enables / Disables this Interaction to be triggered via the LaserComponent. Will also disable Highlighting via the Laser.
	 * The Actor will be ignored during the collision checks in the InteractionSystemComponent.
	 */
	UPROPERTY(EditAnywhere, Category = "XR Interaction|Laser")
	bool bEnableLaserInteraction = true;

	/**
	 * Should this XRInteraction disable the XRLaser while the Interaction is active.
	 */
	UPROPERTY(EditAnywhere, Category = "XR Interaction|Laser")
	bool bSupressLaserWhenInteracting = false;

	/**
	 * Should this XRInteraction force the XRLaser to snap to the OwningActor.
	 */
	UPROPERTY(EditAnywhere, Category = "XR Interaction|Laser")
	bool bSnapXRLaserToActor = false;

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Config - Highlighting
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	/**
	 * Ebanble Material based Highlighting for this Interaction.
	 * Spawn an XRInteractionHighlight on BeginPlay and set this interaction as it`s assigned Interaction if true at BeginPlay.
	 */
	UPROPERTY(EditAnywhere, Category = "XR Interaction|Highlighting")
	bool bEnableHighlighting = true;

	/**
	 * Tag used to determine which MeshComponents to ignore for Highlighting.
	 */
	UPROPERTY(EditAnywhere, Category = "XR Interaction|Highlighting")
	FName HighlightIgnoreMeshTag = "XRHighlight_Ignore";

	/**
	 * Fade the highlight based on this curve. If no curve is provided, HighlightState will be set instantly.
	 */
	UPROPERTY(EditAnywhere, Category = "XR Interaction|Highlighting")
	UCurveFloat* HighlightFadeCurve = nullptr;

	UPROPERTY()
	UXRHighlightComponent* XRHighlightComponent = nullptr;

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Config - Audio
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	/**
	* Played when the Interaction starts at the location of the XRInteractionComponent.
	 */
	UPROPERTY(EditAnywhere, Category = "XR Interaction|Audio")
	USoundBase* InteractionStartSound= nullptr;
	/**
	* Played when the Interaction ends at the location of the XRInteractionComponent.
	 */
	UPROPERTY(EditAnywhere, Category = "XR Interaction|Audio")
	USoundBase* InteractionEndSound = nullptr;

	UPROPERTY()
	UAudioComponent* CurrentAudioComponent = nullptr;

	UFUNCTION()
	void RequestAudioPlay(USoundBase* InSound);
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------

private:
	UFUNCTION()
	void SpawnAndConfigureXRHighlight();
	
	UPROPERTY()
	TArray<UMeshComponent*> InteractionCollision = {nullptr};
	
	UPROPERTY()
	UXRInteractorComponent* ActiveInteractor = nullptr;
};
