#include "InventoryManipulatorComponent.h"

#include "FCTween.h"
#include "InventoryComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Interfaces/ContextActionProvider.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "ProjectHorrorPhoto/AbilitySystem/Input/PhotoAbilitySet.h"
#include "ProjectHorrorPhoto/Components/Physics/Dragger.h"
#include "ProjectHorrorPhoto/Components/Physics/Grabber.h"
#include "ProjectHorrorPhoto/Useful/Useful.h"
#include "Types/ContextAction.h"
#include "Types/EquippedItemActor.h"
#include "Types/InventoryItemInstance.h"
#include "Types/Item.h"
#include "Types/ItemDefinition.h"


class UCameraComponent;

UInventoryManipulatorComponent::UInventoryManipulatorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
	Grabber = CreateDefaultSubobject<UGrabber>(TEXT("Grabber"));
	Grabber->OnGrabbedItemChanged.AddDynamic(this, &UInventoryManipulatorComponent::OnGrabbedOrDraggedActorChanged);

	Dragger = CreateDefaultSubobject<UDragger>(TEXT("Dragger"));
	Dragger->OnDraggedItemChanged.AddDynamic(this, &UInventoryManipulatorComponent::OnGrabbedOrDraggedActorChanged);

	HotbarItems.SetNum(MaxHotbarSlots);
}

bool UInventoryManipulatorComponent::TakeToHands(UInventoryItemInstance* Item)
{
	if (!Item) return false;
	if (!CanHoldInHands(Item)) return false;

	if (EquippedItem)
	{
		EquippedItem->HotbarIndex = -1;
		if (UInventoryComponent* Inv = GetOwner()->FindComponentByClass<UInventoryComponent>())
		{
			Inv->AddItemInstance(EquippedItem);
		}
		DestroyEquippedVisual();
		EquippedItem = nullptr;
	}

	if (UInventoryComponent* Inv = GetOwner()->FindComponentByClass<UInventoryComponent>())
	{
		Inv->ConsumeOne(Item);
	}

	EquippedItem = Item;
	UpdateEquippedVisual();

	UE_LOG(LogTemp, Log, TEXT("Equipped item %s"), *Item->ItemDef->DisplayName.ToString());

	return true;
}

bool UInventoryManipulatorComponent::HoldInHands(UInventoryItemInstance* Item)
{
	if (!Item) return false;
	if (!CanHoldInHands(Item)) return false;

	if (IsValid(GrabbedItem) && GrabbedItem->ItemInstance == Item)
		DropGrabbedItem();

	if (EquippedItem)
	{
		PutToInventory(EquippedItem);
		DestroyEquippedVisual();
		EquippedItem = nullptr;
	}

	EquippedItem = Item;

	// reset 
	if (!(EquippedFromSlot >= 0 && GetHotbarItem(EquippedFromSlot) == nullptr))
	{
		EquippedFromSlot = INDEX_NONE;
	}

	UpdateEquippedVisual();
	UE_LOG(LogTemp, Log, TEXT("Equipped item %s"), *Item->ItemDef->DisplayName.ToString());
	return true;
}


bool UInventoryManipulatorComponent::PutFromHandsToInventory()
{
	if (!EquippedItem) return false;

	UInventoryItemInstance* ToStore = EquippedItem;
	EquippedItem = nullptr;
	DestroyEquippedVisual();

	PutToInventory(ToStore);
	return true;
}

static inline bool SameType(const UInventoryItemInstance* A, const UInventoryItemInstance* B)
{
	return A && B && A->ItemDef == B->ItemDef;
}

static inline bool CanMergeStacks(const UInventoryItemInstance* A, const UInventoryItemInstance* B)
{
	return SameType(A, B) && A->IsIdenticalTo(B) && A->ItemDef->bStackable && B->ItemDef->bStackable;
}


void UInventoryManipulatorComponent::PutToInventory(UInventoryItemInstance* Item)
{
	if (!Item) return;
	if (UInventoryComponent* Inv = GetOwner()->FindComponentByClass<UInventoryComponent>())
	{
		Item->HotbarIndex = -1;

		if (!Inv->AddItemInstance(Item))
		{
			DropInstanceInFrontOfPlayer(Item);
		}
	}
}

UInventoryItemInstance* UInventoryManipulatorComponent::SplitQtyFromStack(
	UInventoryItemInstance* Source, int32 Qty)
{
	check(Source && Qty > 0 && Source->StackCount >= Qty);

	if (Source->StackCount == Qty)
	{
		return Source;
	}

	Source->StackCount -= Qty;

	UInventoryItemInstance* NewInst = NewObject<UInventoryItemInstance>(this);
	NewInst->InitFromInstance(Source);
	NewInst->StackCount = Qty;
	NewInst->HotbarIndex = -1;
	return NewInst;
}

float UInventoryManipulatorComponent::GetHotbarWeight() const
{
	float Sum = 0.f;
	for (const UInventoryItemInstance* It : HotbarItems)
	{
		if (!It) continue;
		Sum += It->GetUnitWeight() * FMath::Max(1, It->StackCount);
	}
	return Sum;
}

float UInventoryManipulatorComponent::GetEquippedWeight() const
{
	return EquippedItem ? EquippedItem->GetUnitWeight() * FMath::Max(1, EquippedItem->StackCount) : 0.f;
}

float UInventoryManipulatorComponent::GetCarriedWeight() const
{
	const UInventoryComponent* Inv = GetOwner()->FindComponentByClass<UInventoryComponent>();
	const float InvW = Inv ? Inv->GetCurrentWeight() : 0.f;
	return InvW + GetHotbarWeight() + GetEquippedWeight();
}

bool UInventoryManipulatorComponent::CanCarryDeltaInHands(float Delta)
{
	return Delta <= MaxHandItemWeight;
}

bool UInventoryManipulatorComponent::CanCarryDeltaInHotbar(float Delta)
{
	return (GetHotbarWeight() + Delta) <= MaxHotbarWeight + KINDA_SMALL_NUMBER;
}

bool UInventoryManipulatorComponent::CanCarryItemInHotbar(const UInventoryItemInstance* Item) const
{
	return Item && (GetHotbarWeight() + Item->GetUnitWeight() * FMath::Max(1, Item->StackCount) <= MaxHotbarWeight +
		KINDA_SMALL_NUMBER);
}


bool UInventoryManipulatorComponent::CanHoldInHands(const UInventoryItemInstance* Item) const
{
	return Item && (Item->GetUnitWeight() <= (MaxHandItemWeight + KINDA_SMALL_NUMBER));
}

bool UInventoryManipulatorComponent::ReturnHeldToSlotPolicy(int32 SlotIndex)
{
	if (!EquippedItem) return false;

	UInventoryComponent* Inv = GetOwner()->FindComponentByClass<UInventoryComponent>();
	if (!Inv || !HotbarItems.IsValidIndex(SlotIndex)) return false;

	UInventoryItemInstance* SlotItem = HotbarItems[SlotIndex];
	const int32 HeldQty = FMath::Max(1, EquippedItem->StackCount);
	const float HeldW = EquippedItem->GetUnitWeight() * HeldQty;

	if (!SlotItem)
	{
		if (GetHotbarWeight() + HeldW <= MaxHotbarWeight + KINDA_SMALL_NUMBER)
		{
			HotbarItems[SlotIndex] = EquippedItem;
			EquippedItem->HotbarIndex = SlotIndex;
		}
		else
		{
			PutToInventory(EquippedItem);
		}

		DestroyEquippedVisual();
		EquippedItem = nullptr;
		EquippedFromSlot = INDEX_NONE;
		OnHotbarUpdated.Broadcast();
		return true;
	}

	if (CanMergeStacks(SlotItem, EquippedItem))
	{
		if (GetHotbarWeight() + HeldW <= MaxHotbarWeight + KINDA_SMALL_NUMBER)
		{
			SlotItem->StackCount += HeldQty;
		}
		else
		{
			PutToInventory(EquippedItem);
		}

		DestroyEquippedVisual();
		EquippedItem = nullptr;
		EquippedFromSlot = INDEX_NONE;
		OnHotbarUpdated.Broadcast();
		return true;
	}

	PutToInventory(EquippedItem);
	DestroyEquippedVisual();
	EquippedItem = nullptr;
	EquippedFromSlot = INDEX_NONE;
	OnHotbarUpdated.Broadcast();
	return true;
}

bool UInventoryManipulatorComponent::DropEquippedItem(const FTransform& SpawnXform)
{
	if (!EquippedItem) return false;

	TSubclassOf<AItem> SpawnClass = EquippedItem->ItemDef->OverrideActorClass
		                                ? EquippedItem->ItemDef->OverrideActorClass
		                                : TSubclassOf<AItem>(AItem::StaticClass());

	AItem* Spawned = GetWorld()->SpawnActorDeferred<AItem>(SpawnClass, SpawnXform, GetOwner());
	if (!Spawned) return false;

	Spawned->InitializeFromInstance(EquippedItem);
	Spawned->FinishSpawning(SpawnXform);

	EquippedItem = nullptr;
	DestroyEquippedVisual();

	return true;
}

bool UInventoryManipulatorComponent::DropEquippedItemInFrontOfPlayer()
{
	if (!EquippedItem) return false;

	if (ACharacter* Char = Cast<ACharacter>(GetOwner()))
	{
		if (UCameraComponent* Cam = Char->FindComponentByClass<UCameraComponent>())
		{
			FVector StartLoc = Cam->GetComponentLocation();
			FVector EndLoc = StartLoc + Cam->GetForwardVector() * HitDistance;
			FHitResult Hit;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(GetOwner());
			if (GetWorld()->LineTraceSingleByChannel(Hit, StartLoc, EndLoc, ECC_Visibility, Params))
			{
				FVector SpawnLoc = Hit.ImpactPoint + Hit.ImpactNormal * 20.f;
				FRotator SpawnRot = Cam->GetComponentRotation();
				FTransform SpawnXform(SpawnRot, SpawnLoc);

				return DropEquippedItem(SpawnXform);
			}
			else
			{
				FVector SpawnLoc = EndLoc;
				FRotator SpawnRot = Cam->GetComponentRotation();
				FTransform SpawnXform(SpawnRot, SpawnLoc);

				return DropEquippedItem(SpawnXform);
			}
		}
	}
	return false;
}

bool UInventoryManipulatorComponent::DropInstanceInFrontOfPlayer(UInventoryItemInstance* Instance, float ForwardDist)
{
	if (!Instance || !Instance->ItemDef) return false;

	if (ACharacter* Char = Cast<ACharacter>(GetOwner()))
	{
		if (UCameraComponent* Cam = Char->FindComponentByClass<UCameraComponent>())
		{
			const FVector SpawnLoc = Cam->GetComponentLocation() + Cam->GetForwardVector() * ForwardDist;
			const FTransform Xform(Cam->GetComponentRotation(), SpawnLoc);

			TSubclassOf<AItem> SpawnClass = Instance->ItemDef->OverrideActorClass
				                                ? Instance->ItemDef->OverrideActorClass
				                                : TSubclassOf<AItem>(AItem::StaticClass());

			AItem* WorldItem = GetWorld()->SpawnActorDeferred<AItem>(SpawnClass, Xform, GetOwner());
			if (!WorldItem) return false;

			WorldItem->InitializeFromInstance(Instance);
			WorldItem->FinishSpawning(Xform);
			return true;
		}
	}
	return false;
}

bool UInventoryManipulatorComponent::ThrowEquippedItem(float ThrowForce)
{
	if (!EquippedItem) return false;

	if (ACharacter* Char = Cast<ACharacter>(GetOwner()))
	{
		if (UCameraComponent* Cam = Char->FindComponentByClass<UCameraComponent>())
		{
			FVector SpawnLoc = Cam->GetComponentLocation() + Cam->GetForwardVector() * 100.f;
			FTransform SpawnXform(Cam->GetComponentRotation(), SpawnLoc);

			TSubclassOf<AItem> SpawnClass = EquippedItem->ItemDef->OverrideActorClass
				                                ? EquippedItem->ItemDef->OverrideActorClass
				                                : TSubclassOf<AItem>(AItem::StaticClass());

			AItem* WorldItem = GetWorld()->SpawnActorDeferred<AItem>(SpawnClass, SpawnXform, GetOwner());
			if (!WorldItem) return false;

			WorldItem->InitializeFromInstance(EquippedItem);
			WorldItem->FinishSpawning(SpawnXform);

			UInventoryComponent* Inv = Char->FindComponentByClass<UInventoryComponent>();
			if (Inv) Inv->RemoveItemInstance(EquippedItem);

			EquippedItem = nullptr;

			DestroyEquippedVisual();
			if (UPrimitiveComponent* Prim = WorldItem->FindComponentByClass<UPrimitiveComponent>())
			{
				if (Prim->IsSimulatingPhysics())
				{
					float Mass = Prim->GetMass();
					float CalculatedThrowForce = ThrowForce / (1.f + Mass / MassFactor);

					FVector ForwardDir = Cam->GetForwardVector();
					Prim->AddImpulse(ForwardDir * CalculatedThrowForce, NAME_None, true);
				}
			}

			return true;
		}
	}
	return false;
}

bool UInventoryManipulatorComponent::GrabEquipped()
{
	if (!EquippedItem) return false;

	if (ACharacter* Char = Cast<ACharacter>(GetOwner()))
	{
		if (UCameraComponent* Cam = Char->FindComponentByClass<UCameraComponent>())
		{
			FVector SpawnLoc = Cam->GetComponentLocation() + Cam->GetForwardVector() * 100.f;
			FTransform SpawnXform(Cam->GetComponentRotation(), SpawnLoc);

			TSubclassOf<AItem> SpawnClass = EquippedItem->ItemDef->OverrideActorClass
				                                ? EquippedItem->ItemDef->OverrideActorClass
				                                : TSubclassOf<AItem>(AItem::StaticClass());

			AItem* WorldItem = GetWorld()->SpawnActor<AItem>(SpawnClass, SpawnXform);
			if (!WorldItem) return false;

			WorldItem->InitializeFromInstance(EquippedItem);

			DestroyEquippedVisual();
			EquippedItem = nullptr;

			return DragOrGrabItem(WorldItem);
		}
	}
	return false;
}

bool UInventoryManipulatorComponent::EquipGrabbed()
{
	if (!GrabbedItem) return false;

	UInventoryItemInstance* Instance = GrabbedItem->ItemInstance;
	if (!Instance || !Instance->ItemDef) return false;

	const float AddWeight = Instance->GetUnitWeight() * FMath::Max(1, Instance->StackCount);
	if (!CanCarryDeltaInHands(AddWeight))
	{
		DropGrabbedItem();
		return false;
	}

	UInventoryComponent* Inv = GetOwner()->FindComponentByClass<UInventoryComponent>();
	if (!Inv || !Inv->CanAcceptInstance(Instance))
	{
		DropGrabbedItem();
		return false;
	}

	AItem* ItemToDestroy = GrabbedItem;
	DropGrabbedItem();

	if (IsValid(ItemToDestroy))
		ItemToDestroy->Destroy();

	Inv->AddItemInstance(Instance);

	if (CanHoldInHands(Instance))
	{
		return TakeToHands(Instance);
	}

	return true;
}

bool UInventoryManipulatorComponent::ActivateHotbarSlot(int32 SlotIndex)
{
	if (SlotIndex < 0) return false;

	UInventoryComponent* Inv = GetOwner()->FindComponentByClass<UInventoryComponent>();
	if (!Inv) return false;

	UInventoryItemInstance* SlotItem = GetHotbarItem(SlotIndex);

	// autoretun
	if (EquippedItem && EquippedFromSlot == SlotIndex)
	{
		return ReturnHeldToSlotPolicy(SlotIndex);
	}

	// emtpy slot
	if (!SlotItem)
	{
		if (EquippedItem && EquippedFromSlot == SlotIndex)
		{
			return ReturnHeldToSlotPolicy(SlotIndex);
		}

		if (EquippedItem)
		{
			if (!CanCarryItemInHotbar(EquippedItem))
			{
				PutToInventory(EquippedItem);
				DestroyEquippedVisual();
				EquippedItem = nullptr;
				EquippedFromSlot = INDEX_NONE;
				OnHotbarUpdated.Broadcast();
				return true;
			}

			HotbarItems[SlotIndex] = EquippedItem;
			EquippedItem->HotbarIndex = SlotIndex;

			DestroyEquippedVisual();
			EquippedItem = nullptr;
			EquippedFromSlot = INDEX_NONE;

			OnHotbarUpdated.Broadcast();
			OnHotbarSlotUpdated.Broadcast(SlotIndex, HotbarItems[SlotIndex]); // if you use it
			return true;
		}

		return false;
	}

	// grab from hotbar
	if (!EquippedItem)
	{
		if (!CanHoldInHands(SlotItem))
			return false;

		UInventoryItemInstance* ToHand = SplitQtyFromStack(SlotItem, /*Qty*/ 1);

		if (ToHand == SlotItem) // full stack taken
		{
			HotbarItems[SlotIndex] = nullptr;
			ToHand->HotbarIndex = -1;
		}


		EquippedFromSlot = SlotIndex; // mark source
		OnHotbarUpdated.Broadcast();
		return HoldInHands(ToHand);
	}

	// merge if same
	if (CanMergeStacks(SlotItem, EquippedItem))
	{
		if (!CanCarryItemInHotbar(EquippedItem))
		{
			PutToInventory(EquippedItem);
			DestroyEquippedVisual();
			EquippedItem = nullptr;
			EquippedFromSlot = INDEX_NONE;
			OnHotbarUpdated.Broadcast();
			return true;
		}
		SlotItem->StackCount += EquippedItem->StackCount;
		DestroyEquippedVisual();
		EquippedItem = nullptr;
		EquippedFromSlot = INDEX_NONE;
		OnHotbarUpdated.Broadcast();
		return true;
	}

	// different - hands -> inventory
	PutToInventory(EquippedItem);
	DestroyEquippedVisual();
	EquippedItem = nullptr;
	EquippedFromSlot = INDEX_NONE;

	// get only one piece
	{
		UInventoryItemInstance* ToHand = SplitQtyFromStack(SlotItem, /*Qty*/ 1);
		if (ToHand == SlotItem)
		{
			HotbarItems[SlotIndex] = nullptr;
			ToHand->HotbarIndex = -1;
		}
		EquippedFromSlot = SlotIndex;
		OnHotbarUpdated.Broadcast();
		return HoldInHands(ToHand);
	}
}

bool UInventoryManipulatorComponent::AssignHotbarSlot(int32 SlotIndex, UInventoryItemInstance* Item)
{
	if (SlotIndex < 0 || !Item) return false;

	UInventoryComponent* Inv = GetOwner()->FindComponentByClass<UInventoryComponent>();
	if (!Inv) return false;

	UInventoryItemInstance* SlotItem = (HotbarItems.IsValidIndex(SlotIndex) ? HotbarItems[SlotIndex] : nullptr);

	const int32 ItemQty = FMath::Max(1, Item->StackCount);
	const float ItemW = Item->GetUnitWeight() * ItemQty;
	const float SlotW = SlotItem ? SlotItem->GetUnitWeight() * FMath::Max(1, SlotItem->StackCount) : 0.f;

	// if merge: hotbar gains only ItemW
	if (SlotItem && CanMergeStacks(SlotItem, Item))
	{
		const float NewHB = GetHotbarWeight() + ItemW;
		if (NewHB > MaxHotbarWeight + KINDA_SMALL_NUMBER)
		{
			DropInstanceInFrontOfPlayer(Item);
			return false;
		}

		if (Inv->Items.Contains(Item))
		{
			Inv->RemoveItemInstance(Item, Item->StackCount);
		}
		SlotItem->StackCount += Item->StackCount;
		OnHotbarUpdated.Broadcast();
		return true;
	}


	const float NewHB = GetHotbarWeight() - SlotW + ItemW;
	if (NewHB > MaxHotbarWeight + KINDA_SMALL_NUMBER)
		return false;


	if (SlotItem)
	{
		SlotItem->HotbarIndex = -1;
		if (!Inv->AddItemInstance(SlotItem))
		{
			DropInstanceInFrontOfPlayer(SlotItem);
		}
	}


	if (Inv->Items.Contains(Item))
	{
		Inv->RemoveItemInstance(Item, Item->StackCount);
	}
	HotbarItems[SlotIndex] = Item;
	Item->HotbarIndex = SlotIndex;

	OnHotbarUpdated.Broadcast();
	return true;
}

UInventoryItemInstance* UInventoryManipulatorComponent::GetHotbarItem(int32 SlotIndex)
{
	if (SlotIndex < 0) return nullptr;
	if (HotbarItems.IsValidIndex(SlotIndex))
	{
		return HotbarItems[SlotIndex];
	}
	return nullptr;
}

bool UInventoryManipulatorComponent::ExecuteHeldItemAction(const FGameplayTag& ActionTag, FName ActionId)
{
	if (!EquippedItem) return false;

	FContextActionContext Ctx;
	Ctx.Caller = GetOwner();
	Ctx.SourceItem = EquippedItem;
	Ctx.ActionTag = ActionTag;
	Ctx.ActionId = ActionId;

	return IContextActionProvider::Execute_ExecuteAction(EquippedItem, ActionTag, ActionId, Ctx);
}

bool UInventoryManipulatorComponent::ExecuteWorldTargetAction(AActor* TargetActor, const FGameplayTag& ActionTag,
                                                              FName ActionId)
{
	if (!TargetActor || !TargetActor->GetClass()->ImplementsInterface(UContextActionProvider::StaticClass()))
		return false;

	FContextActionContext Ctx;
	Ctx.Caller = GetOwner();
	Ctx.SourceActor = TargetActor;
	Ctx.ActionTag = ActionTag;
	Ctx.ActionId = ActionId;

	return IContextActionProvider::Execute_ExecuteAction(TargetActor, ActionTag, ActionId, Ctx);
}

static FCTweenInstanceVector* GTweenInstance = nullptr;
static USceneComponent* GAttachPoint;

void UInventoryManipulatorComponent::MoveItemInFrontOfPlayer(bool bInFront)
{
	if (!EquippedVisual) return;

	if (ACharacter* Char = Cast<ACharacter>(GetOwner()))
	{
		if (UCameraComponent* Cam = Char->FindComponentByClass<UCameraComponent>())
		{
			
			if (bInFront)
			{
				GAttachPoint = UUseful::FindChildComponentByTag(Char, FName("ItemFrontFacePoint"));
			}
			else
			{
				GAttachPoint = UUseful::FindChildComponentByTag(Char, FName("ItemVisualEquipmentPoint"));
			}

			if (GAttachPoint)
			{
				auto OnCompleteCallback = [this]()
				{
					if (EquippedVisual)
					{
						EquippedVisual->AttachToComponent(
							GAttachPoint,
							FAttachmentTransformRules::SnapToTargetNotIncludingScale
						);
					}
				};

				GTweenInstance = FCTween::Play(
					EquippedVisual->GetActorLocation(),
					GAttachPoint->GetComponentLocation(),
					[&](FVector t)
					{
						if (EquippedVisual)
						{
							EquippedVisual->SetActorLocation(t);
						}
					},
					0.2f,
					EFCEase::OutCubic);
				GTweenInstance->SetOnComplete(OnCompleteCallback);
			}
		}
	}
}

void UInventoryManipulatorComponent::UpdateEquippedVisual()
{
	DestroyEquippedVisual();

	if (!EquippedItem) return;

	if (ACharacter* Char = Cast<ACharacter>(GetOwner()))
	{
		FActorSpawnParameters Params;
		Params.Owner = Char;

		AEquippedItemActor* Visual = GetWorld()->SpawnActor<AEquippedItemActor>(
			AEquippedItemActor::StaticClass(),
			Params
		);

		USceneComponent* AttachPoint = UUseful::FindChildComponentByTag(Char, FName("ItemVisualEquipmentPoint"));

		Visual->AttachToComponent(AttachPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

		Visual->InitFromInstance(EquippedItem);

		EquippedVisual = Visual;


		UPhotoAbilitySystemComponent* ASC = UUseful::GetPhotoAbilitySystemComponent(GetOwner());
		if (ASC && EquippedItem->ItemDef && EquippedItem->ItemDef->GrantedAbilitySet)
		{
			EquippedItem->ItemDef->GrantedAbilitySet->GiveToAbilitySystem(ASC, EquippedGrantedHandles);
		}
	}
}

void UInventoryManipulatorComponent::DestroyEquippedVisual()
{
	if (EquippedVisual)
	{
		UPhotoAbilitySystemComponent* ASC = UUseful::GetPhotoAbilitySystemComponent(GetOwner());
		if (ASC && EquippedGrantedHandles)
		{
			EquippedGrantedHandles->TakeFromAbilitySystem(ASC);
			EquippedGrantedHandles = nullptr;
		}

		EquippedVisual->Destroy();
		EquippedVisual = nullptr;
	}
}


bool UInventoryManipulatorComponent::DragOrGrabItem(AItem* WorldItem)
{
	if (!WorldItem) return false;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());
	if (ACharacter* Char = Cast<ACharacter>(GetOwner()))
	{
		if (UCameraComponent* Cam = Char->FindComponentByClass<UCameraComponent>())
		{
			FVector Start = Cam->GetComponentLocation();
			FVector End = Start + Cam->GetForwardVector() * (HitDistance);

			if (!GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
			{
				return false;
			}
		}
	}

	if (WorldItem->Mesh)
	{
		float Mass = WorldItem->Mesh->GetMass();
		if (Mass <= MaxGrabMass)
		{
			GrabbedItem = WorldItem;
			GrabMode = EGrabMode::Grab;
			return Grabber->Grab(WorldItem, Hit);
		}
		else if (Mass <= MaxDragMass)
		{
			GrabbedItem = WorldItem;
			GrabMode = EGrabMode::Drag;
			return Dragger->DragItem(WorldItem, Hit);
		}
	}

	return false;
}

void UInventoryManipulatorComponent::DropGrabbedItem()
{
	if (!GrabbedItem) return;

	if (GrabMode == EGrabMode::Grab)
	{
		Grabber->DropGrabbed();
	}
	else if (GrabMode == EGrabMode::Drag)
	{
		Dragger->DropDragged();
	}

	GrabbedItem = nullptr;
}

void UInventoryManipulatorComponent::ThrowGrabbedItem(float ThrowForce)
{
	if (GrabMode != EGrabMode::Grab) return;
	Grabber->ThrowGrabbed(ThrowForce);
	GrabbedItem = nullptr;
}


void UInventoryManipulatorComponent::OnGrabbedOrDraggedActorChanged(AActor* NewGrabbedItem)
{
	if (NewGrabbedItem == nullptr)
	{
		GrabbedItem = nullptr;
		return;
	}
}

void UInventoryManipulatorComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GTweenInstance)
	{
		GTweenInstance->EndValue = GAttachPoint ? GAttachPoint->GetComponentLocation() : FVector::ZeroVector;
	}
}
