#include "XRInteractionGrab.h"
#include "XRReplicatedPhysicsComponent.h"
#include "XRInteractorComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"


UXRInteractionGrab::UXRInteractionGrab()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	bAutoActivate = true;
	SetIsReplicated(true);
}

void UXRInteractionGrab::BeginPlay()
{
	Super::BeginPlay();
	if (bEnablePhysics)
	{
		InitializePhysics();
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Interaction Events
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRInteractionGrab::StartInteraction(UXRInteractorComponent* InInteractor)
{
	Super::StartInteraction(InInteractor);
	if (bEnablePhysics)
	{
		PhysicsGrab(InInteractor);
	}
	else
	{
		AttachOwningActorToXRInteractor(InInteractor);
	}
}

void UXRInteractionGrab::EndInteraction(UXRInteractorComponent* InInteractor)
{
	Super::EndInteraction(InInteractor);
	if (bEnablePhysics)
	{
		PhysicsUngrab(InInteractor);
	}
	else
	{
		DetachOwningActorFromXRInteractor();
	}
}


// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Grab functions
// ------------------------------------------------------------------------------------------------------------------------------------------------------------

void UXRInteractionGrab::AttachOwningActorToXRInteractor(UXRInteractorComponent* InInteractor)
{
	if (InInteractor)
	{
		FAttachmentTransformRules Rules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, false);
		GetOwner()->AttachToComponent(InInteractor, Rules);
	}
}

void UXRInteractionGrab::DetachOwningActorFromXRInteractor()
{
	FDetachmentTransformRules Rules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false);
	GetOwner()->DetachFromActor(Rules);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Physics
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRInteractionGrab::PhysicsGrab(UXRInteractorComponent* InInteractor)
{
	if (InInteractor)
	{
		if (GetOwner()->HasAuthority())
		{
			XRReplicatedPhysicsComponent->Server_SetServerReplicatePhysics(false);
		}
		AttachOwningActorToXRInteractor(InInteractor);
		XRReplicatedPhysicsComponent->SetComponentsSimulatePhysics(true);

		TArray<UMeshComponent*> MeshComponents = XRReplicatedPhysicsComponent->GetPhysicsMeshComponents();
		if (MeshComponents.Num() > 0)
		{
			UMeshComponent* PhysicsEnabledMesh = MeshComponents[0];
			UPhysicsConstraintComponent* ActivePhysicsConstraint = InInteractor->GetAssignedPhysicsConstraint();

			if (ActivePhysicsConstraint && PhysicsEnabledMesh)
			{
				ActivePhysicsConstraint->SetConstrainedComponents(InInteractor, "", PhysicsEnabledMesh, "");
			}
		}
	}
}

void UXRInteractionGrab::PhysicsUngrab(UXRInteractorComponent* InInteractor)
{
	if (InInteractor)
	{
		UPhysicsConstraintComponent* ActivePhysicsConstraint = InInteractor->GetAssignedPhysicsConstraint();
		if (ActivePhysicsConstraint)
		{
			ActivePhysicsConstraint->BreakConstraint();
		}
	}
	if (GetOwner()->HasAuthority())
	{
		XRReplicatedPhysicsComponent->Server_SetServerReplicatePhysics(true);
	}
}

void UXRInteractionGrab::InitializePhysics()
{
	AActor* Owner = GetOwner();
	if (Owner)
	{
		UXRReplicatedPhysicsComponent* FoundXRPhysicsComponent = Owner->FindComponentByClass<UXRReplicatedPhysicsComponent>();
		if (FoundXRPhysicsComponent)
		{
			XRReplicatedPhysicsComponent = FoundXRPhysicsComponent;
			return;
		}
		else
		{
			XRReplicatedPhysicsComponent = NewObject<UXRReplicatedPhysicsComponent>(this->GetOwner());
			if (XRReplicatedPhysicsComponent)
			{
				XRReplicatedPhysicsComponent->RegisterComponent();
				XRReplicatedPhysicsComponent->Activate();
				XRReplicatedPhysicsComponent->CachePhysicsMeshComponents(PhysicsTag);
				if (Owner->HasAuthority())
				{
					XRReplicatedPhysicsComponent->Server_SetServerReplicatePhysics(true);
				}
			}
		}
	}
}


UXRReplicatedPhysicsComponent* UXRInteractionGrab::GetPhysicsReplicationComponent()
{
	return XRReplicatedPhysicsComponent;
}