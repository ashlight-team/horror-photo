#include "MassBasedGravityComponent.h"


UMassBasedGravityComponent::UMassBasedGravityComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UMassBasedGravityComponent::BeginPlay()
{
	Super::BeginPlay();

	if (bOverrideDefaultGravity)
	{
		if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent()))
		{
			Prim->SetEnableGravity(false); 
		}
	}
}


void UMassBasedGravityComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                               FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent()))
	{
		float Mass = Prim->GetMass();
		
		FVector Force = FVector(0, 0, -GravityStrength * Mass);
		Prim->AddForce(Force);
	}
}

