// Fill out your copyright notice in the Description page of Project Settings.


#include "Dragger.h"

#include "ProjectHorrorPhoto/AbilitySystem/PhotoGameplayTags.h"
#include "ProjectHorrorPhoto/AbilitySystem/Attributes/MovementAttributeSet.h"
#include "ProjectHorrorPhoto/AbilitySystem/Component/PhotoAbilitySystemComponent.h"
#include "ProjectHorrorPhoto/Useful/Useful.h"


UDragger::UDragger()
{
	PrimaryComponentTick.bCanEverTick = true;
	DragOffset = FVector::ZeroVector;
}

bool UDragger::DragItem(AActor* WorldActor, const FHitResult& HitResult)
{
	if (!WorldActor || !HandAnchor) return false;

	DropDragged();

	DraggedItem = WorldActor;

	DragOffset = WorldActor->GetActorTransform().InverseTransformPosition(HitResult.ImpactPoint);
	HandAnchor->SetWorldLocation(HitResult.ImpactPoint);

	UPhotoAbilitySystemComponent* ASC = UUseful::GetPhotoAbilitySystemComponent(GetOwner());
	if (ASC) ASC->AddLooseGameplayTag(FPhotoGameplayTags::Get().Status_Item_Dragging);

	OnDraggedItemChanged.Broadcast(DraggedItem);
	return true;
}

void UDragger::DropDragged()
{
	if (!DraggedItem) return;

	UPhotoAbilitySystemComponent* ASC = UUseful::GetPhotoAbilitySystemComponent(GetOwner());
	if (ASC)
	{
		ASC->SetNumericAttributeBase(UMovementAttributeSet::GetItemDragCoefficientAttribute(), 1.f);
		ASC->RemoveLooseGameplayTag(FPhotoGameplayTags::Get().Status_Item_Dragging);
	}

	DraggedItem = nullptr;
	OnDraggedItemChanged.Broadcast(DraggedItem);
}


void UDragger::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner())
	{
		HandAnchor = Cast<UPrimitiveComponent>(UUseful::FindChildComponentByTag(Owner, FName("ItemHandAnchor")));
	}
}


void UDragger::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
	if (!DraggedItem || !HandAnchor)
	{
		GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::Red,
		                                 FString::Printf(
			                                 TEXT("[DragTick] Exit: DraggedItem=%p HandAnchor=%p"), DraggedItem,
			                                 HandAnchor));
		return;
	}

	UPrimitiveComponent* DraggedPrim = Cast<UPrimitiveComponent>(
		DraggedItem ? DraggedItem->GetComponentByClass(UPrimitiveComponent::StaticClass()) : nullptr);

	if (!DraggedPrim || !DraggedPrim->IsSimulatingPhysics())
	{
		if (DraggedItem)
		{
			GEngine->AddOnScreenDebugMessage(2, 0.f, FColor::Red,
											 FString::Printf(
												 TEXT("[DragTick] Drop: DraggedItem=%p DraggedPrim=%p Simulating=%d"),
												 DraggedItem, DraggedPrim,
												 DraggedPrim ? DraggedPrim->IsSimulatingPhysics() : 0));
			DropDragged();
		}
		return;
	}

	const FTransform CompT = DraggedPrim->GetComponentTransform();
	const FVector GrabPoint = CompT.TransformPosition(DragOffset);

	DrawDebugBox(GetWorld(), GrabPoint, FVector(5.f), FColor::Blue, false, 0.f, 0, 2.f);

	const FVector Target = HandAnchor->GetComponentLocation();

	DrawDebugBox(GetWorld(), Target, FVector(5.f), FColor::Red, false, 0.f, 0, 2.f);

	const FVector Delta = Target - GrabPoint;
	const float Dist = Delta.Length();

	DrawDebugLine(GetWorld(), GrabPoint, Target, FColor::Green, false, 0.f, 0, 1.5f);


	GEngine->AddOnScreenDebugMessage(12, 0.f, FColor::Green,
	                                 FString::Printf(
		                                 TEXT("[DragTick] Dist=%.2f Hold=%.2f Slack=%.2f"), Dist, DragDistance,
		                                 Slack));

	if (Dist <= DragDistance)
	{
		GEngine->AddOnScreenDebugMessage(13, 0.f, FColor::Yellow,
		                                 FString::Printf(
			                                 TEXT("[DragTick] Early exit: within slack (%.2f <= %.2f)"), Dist,
			                                 DragDistance + Slack));
		return;
	}

	if (Dist > MaxDragDistance)
	{
		GEngine->AddOnScreenDebugMessage(11, 0.f, FColor::Red,
		                                 FString::Printf(
			                                 TEXT("[DragTick] Drop: too far (%.2f > %.2f)"), Dist,
			                                 MaxDragDistance));
		DropDragged();
		return;
	}

	const float g = -GetWorld()->GetGravityZ() * 0.01f;
	const float Mass = DraggedPrim->GetMass();


	GEngine->AddOnScreenDebugMessage(14, 0.f, FColor::Cyan,
	                                 FString::Printf(TEXT("[DragTick] g=%.2f Mass=%.2f"), g, Mass));

	float OverDist = Dist - DragDistance;

	if (OverDist <= 0.f)
	{
		return;
	}

	float TensionAlpha = FMath::Clamp(OverDist / Slack, 0.f, 1.f);
	FVector PullDir = Delta.GetSafeNormal();
	FVector SpringF = PullDir * FMath::Min(MaxHandForceN, SpringK * OverDist * TensionAlpha);

	const FVector VelAtPoint = DraggedPrim->GetPhysicsLinearVelocityAtPoint(GrabPoint);
	FVector DampF = -VelAtPoint * DampingC;

	FVector FpullUnclamped = SpringF + DampF;
	FVector Fpull = FpullUnclamped;

	const float Weight = Mass * g * GravityScaleWhileGrab;

	float Ratio = FMath::Clamp(Fpull.Size() / Weight, 0.1f, 1.f);


	if (!Fpull.IsNearlyZero())
	{
		DrawDebugLine(GetWorld(), GrabPoint, GrabPoint + (Fpull * 0.01f), FColor::Magenta, false, 0.f, 0, 2.f);
		DrawDebugSphere(GetWorld(), GrabPoint + (Fpull * 0.01f), 5.f, 8, FColor::Magenta, false, 0.f);
	}

	if (!VelAtPoint.IsNearlyZero())
	{
		DrawDebugLine(GetWorld(), GrabPoint, GrabPoint + (VelAtPoint * 0.01f), FColor::Cyan, false, 0.f, 0, 1.5f);
	}

	FVector Horizontal = FVector(Fpull.X, Fpull.Y, 0.f);
	float HorzSize = FMath::Min(Horizontal.Size(), MaxHandForceN);
	Horizontal = Horizontal.GetSafeNormal() * HorzSize;

	float Z = FMath::Clamp(Fpull.Z, -MaxHandForceN, MaxHandForceN);


	if (Fpull.Size() > MaxHandForceN)
	{
		Fpull = Fpull.GetSafeNormal() * MaxHandForceN;

		GEngine->AddOnScreenDebugMessage(15, 0.f, FColor::Red,
		                                 FString::Printf(
			                                 TEXT("[DragTick] Clamp: Fpull=%.2f Max=%.2f"), FpullUnclamped.Size(),
			                                 MaxHandForceN));
	}


	GEngine->AddOnScreenDebugMessage(16, 0.f, FColor::White,
	                                 FString::Printf(TEXT("[DragTick] SpringF=%s DampF=%s Fpull=%s"),
	                                                 *SpringF.ToString(), *DampF.ToString(), *Fpull.ToString()));

	if (Z > 0.f && Z < Weight)
	{
		float FpullRatio = Z / Weight; // пропорция
		Z = Weight * FpullRatio * 0.5f;

		GEngine->AddOnScreenDebugMessage(17, 0.f, FColor::Yellow,
		                                 FString::Printf(
			                                 TEXT("[DragTick] Zero Z: Weight=%.2f, Z=%.2f"), Weight, Z));
	}

	Fpull = FVector(Horizontal.X, Horizontal.Y, Z);


	const FVector FgravExtra(0, 0, -Mass * g * (GravityScaleWhileGrab - 1.f));

	DraggedPrim->AddForceAtLocation(Fpull, GrabPoint, NAME_None);
	DraggedPrim->AddForce(FgravExtra, NAME_None);


	GEngine->AddOnScreenDebugMessage(18, 0.f, FColor::Magenta,
	                                 FString::Printf(
		                                 TEXT("[DragTick] Applied Fpull=%s GravExtra=%s"), *Fpull.ToString(),
		                                 *FgravExtra.ToString()));

	float StrengthCoef = FMath::Clamp(MaxHandForceN / (Weight + 1.f), 0.01f, 1.f);


	GEngine->AddOnScreenDebugMessage(19, 0.f, FColor::Orange,
	                                 FString::Printf(TEXT("[DragTick] StrengthCoef=%.3f (Fpull=%.2f / Max=%.2f)"),
	                                                 StrengthCoef, Fpull.Size(), MaxHandForceN));

	if (UPhotoAbilitySystemComponent* ASC = UUseful::GetPhotoAbilitySystemComponent(GetOwner()))
	{
		ASC->SetNumericAttributeBase(UMovementAttributeSet::GetItemDragCoefficientAttribute(), StrengthCoef);

		GEngine->AddOnScreenDebugMessage(110, 0.f, FColor::Silver,
		                                 FString::Printf(TEXT("[DragTick] ASC->SetCoef=%.3f"), StrengthCoef));
	}
}
