#include "Grabber.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

UGrabber::UGrabber()
{
	PrimaryComponentTick.bCanEverTick = true;
	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
	GrabbedActor = nullptr;
}

bool UGrabber::Grab(AActor* WorldActor, const FHitResult& HitResult)
{
	if (!WorldActor) return false;
	UPrimitiveComponent* MeshComp = Cast<UPrimitiveComponent>(WorldActor->GetComponentByClass(UPrimitiveComponent::StaticClass()));
	if (!MeshComp || !MeshComp->IsSimulatingPhysics()) return false;
	if (MeshComp->GetMass() > MaxGrabMass) return false;
	
	if (GrabbedActor) DropGrabbed();

	float Mass = MeshComp->GetMass();

	PhysicsHandle->bRotationConstrained = true;
	PhysicsHandle->GrabComponentAtLocationWithRotation(
		MeshComp,
		NAME_None,
		WorldActor->GetActorLocation(),
		WorldActor->GetActorRotation()
	);
	MeshComp->SetCollisionProfileName(TEXT("HeldItem"));
	GrabbedActor = WorldActor;

	OnGrabbedItemChanged.Broadcast(GrabbedActor);
	return true;
}

void UGrabber::Rotate(FVector2D Delta)
{
	if (!PhysicsHandle || !PhysicsHandle->GrabbedComponent)
		return;
	
	
	FRotator CurrentRot = GrabbedActor->GetActorRotation();

	FRotator AddRot(Delta.Y * RotationSpeed, Delta.X * RotationSpeed, 0.f);
	FRotator NewRot = CurrentRot + AddRot;
	
	PhysicsHandle->SetTargetRotation(NewRot);
}

void UGrabber::DropGrabbed()
{
	if (!GrabbedActor || !PhysicsHandle || !PhysicsHandle->GrabbedComponent) return;

	UPrimitiveComponent* Comp = Cast<UPrimitiveComponent>(PhysicsHandle->GrabbedComponent);
	if (!Comp) return;

	Comp->SetCollisionProfileName(TEXT("PhysicsActor"));
	PhysicsHandle->ReleaseComponent();

	GrabbedActor = nullptr;
	OnGrabbedItemChanged.Broadcast(GrabbedActor);
}

void UGrabber::ThrowGrabbed(float ThrowForce)
{
	if (!GrabbedActor || !PhysicsHandle || !PhysicsHandle->GrabbedComponent)
		return;
	

	UPrimitiveComponent* Comp = PhysicsHandle->GrabbedComponent;
	if (!Comp) return;

	PhysicsHandle->ReleaseComponent();

	float Mass = Comp->GetMass();
	float CalculatedThrowForce = BaseThrowForce / (1.f + Mass / MassFactor);

	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	if (OwnerChar)
	{
		if (UCameraComponent* Cam = OwnerChar->FindComponentByClass<UCameraComponent>())
		{
			Comp->SetCollisionProfileName(TEXT("PhysicsActor"));

			FVector ForwardDir = Cam->GetForwardVector();
			Comp->AddImpulse(ForwardDir * CalculatedThrowForce, NAME_None, true);
		}
	}

	GrabbedActor = nullptr;
	OnGrabbedItemChanged.Broadcast(GrabbedActor);
}

void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!GrabbedActor || !PhysicsHandle || !PhysicsHandle->GrabbedComponent) return;
	
	float DistanceToItem = FVector::Dist(GrabbedActor->GetActorLocation(), GetOwner()->GetActorLocation());
	GEngine->AddOnScreenDebugMessage(10, 0.f, FColor::Green, FString::Printf(TEXT("Distance to item: %.1f"), DistanceToItem));
	
	if (DistanceToItem > MaxHoldDistance)
	{
		DropGrabbed();
		return;
	}

	if (ACharacter* Char = Cast<ACharacter>(GetOwner()))
	{
		if (UCameraComponent* Cam = Char->FindComponentByClass<UCameraComponent>())
		{
			UPrimitiveComponent* GrabbedMesh = Cast<UPrimitiveComponent>(PhysicsHandle->GrabbedComponent);
			
			FVector Start = Cam->GetComponentLocation();
			FVector DesiredEnd = Start + Cam->GetForwardVector() * HoldDistance;

			FBox Bounds = GrabbedActor->GetComponentsBoundingBox();
			FVector Extents = Bounds.GetExtent();

			FHitResult Hit;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(Char);
			Params.AddIgnoredActor(GrabbedActor);

			bool bHit = GetWorld()->SweepSingleByChannel(
				Hit,
				Start,
				DesiredEnd,
				FQuat::Identity,
				ECC_Visibility,
				FCollisionShape::MakeBox(Extents),
				Params
			);

			FVector TargetLocation = bHit ? Hit.Location - Cam->GetForwardVector() * 1.f : DesiredEnd;

			float Mass = GrabbedMesh->GetMass();
			float EffectiveInterpSpeed = BaseInterpSpeed / (1.f + Mass / MassFactor);

			FVector CurrentLoc = GrabbedActor->GetActorLocation();
			FVector NewLoc = FMath::VInterpTo(CurrentLoc, TargetLocation, DeltaTime, EffectiveInterpSpeed);

			PhysicsHandle->SetTargetLocationAndRotation(NewLoc, GrabbedActor->GetActorRotation());

		}
	}
}

void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	// create physics handle on owner if not set
	if (!IsValid(PhysicsHandle))
	{
		if (AActor* Owner = GetOwner())
		{
			PhysicsHandle = Owner->FindComponentByClass<UPhysicsHandleComponent>();
			if (!IsValid(PhysicsHandle))
			{
				PhysicsHandle = NewObject<UPhysicsHandleComponent>(Owner, UPhysicsHandleComponent::StaticClass(), TEXT("PhysicsHandle"));
				if (IsValid(PhysicsHandle))
				{
					PhysicsHandle->RegisterComponent();
					UE_LOG(LogTemp, Warning, TEXT("Grabber: Created PhysicsHandle component on %s"), *Owner->GetName());
				}
			}
		}
	}
}

