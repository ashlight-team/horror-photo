// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"


#include "Item.h"
#include "InventoryItemInstance.h"
#include "ItemDefinition.h"
#include "ItemViewActor.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "ProjectHorrorPhoto/Components/Inventory/InventoryComponent.h"
#include "ProjectHorrorPhoto/Components/Inventory/InventoryManipulatorComponent.h"

AItem::AItem()
{
	bReplicates = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetHiddenInGame(true);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetSimulatePhysics(true);

	SetRootComponent(Mesh);
	PrimaryActorTick.bCanEverTick = true;
}

void AItem::Destroyed()
{
	if (IsValid(ViewActor)) ViewActor->Destroy();
	Super::Destroyed();
}

void AItem::InitializeFromInstance(UInventoryItemInstance* Instance)
{
	ItemInstance = Instance;
	ItemDef = Instance ? Instance->ItemDef : nullptr;

	RegisterDefaultActions();
	ApplyVisuals();
}

void AItem::RegisterDefaultActions()
{
	if (bRegisteredDefaultActions) return;

	RegisterActionBP(FGameplayTag::RequestGameplayTag("Item.Pickup"),
	                 "Pickup",
	                 FText::FromString("To Inventory"),
	                 this,
	                 FName("BP_OnPickup"));

	RegisterActionBP(FGameplayTag::RequestGameplayTag("Item.Equip"),
	                 "Equip",
	                 FText::FromString("To Hands"),
	                 this,
	                 FName("BP_OnEquip"));

	RegisterActionBP(FGameplayTag::RequestGameplayTag("Item.Grab"),
	                 "Grab",
	                 FText::FromString("Grab"),
	                 this,
	                 FName("BP_OnGrab"));

	bRegisteredDefaultActions = true;
}

void AItem::BeginPlay()
{
	Super::BeginPlay();

	if (!ItemInstance && (ItemDef || DefaultItemDef))
	{
		UItemDefinition* Def = ItemDef ? ItemDef.Get() : DefaultItemDef;

		ItemInstance = NewObject<UInventoryItemInstance>(this);
		ItemInstance->ItemDef = Def;
		ItemInstance->StackCount = 1;

		UE_LOG(LogTemp, Warning, TEXT("%s auto-created ItemInstance for %s"), *GetName(), *Def->DisplayName.ToString());
	}

	RegisterDefaultActions();
	ApplyVisuals();
}

void AItem::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AItem::ApplyVisuals()
{
	Mesh->SetStaticMesh(ItemInstance && ItemInstance->ItemDef ? ItemInstance->ItemDef->StaticMesh : nullptr);
	Mesh->SetMassOverrideInKg(NAME_None, ItemInstance && ItemInstance->ItemDef
		                                     ? ItemInstance->ItemDef->BaseStats.Weight
		                                     : 1.f);

	if (ItemInstance && ItemInstance->ItemDef && ItemInstance->ItemDef->WorldViewClass)
	{
		if (!IsValid(ViewActor) || !ViewActor->IsA(ItemInstance->ItemDef->WorldViewClass))
		{
			if (IsValid(ViewActor)) ViewActor->Destroy();

			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			Params.Owner = this;
			ViewActor = GetWorld()->SpawnActor<AItemViewActor>(ItemInstance->ItemDef->WorldViewClass,
			                                                   GetActorTransform(), Params);
			if (IsValid(ViewActor))
			{
				ViewActor->AttachToActor(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
				ViewActor->InitFromInstance(ItemInstance);
			}
		}
		else
		{
			if (IsValid(ViewActor))
				ViewActor->InitFromInstance(ItemInstance);
		}
	}
	else
	{
		if (IsValid(ViewActor)) ViewActor->Destroy();
	}
}

void AItem::RegisterActionBP(FGameplayTag Tag, FName ActionId, FText Name, UObject* Target, FName Fn, UTexture2D* Icon,
                             bool bEnabled)
{
	FContextAction A;
	A.ActionTag = Tag;
	A.ActionId = ActionId;
	A.DisplayName = Name;
	A.Icon = Icon;
	A.bEnabled = bEnabled;
	A.Target = Target;
	A.FunctionName = Fn;
	WorldActions.Add(MoveTemp(A));
}

TArray<FContextAction> AItem::GetContextActions_Implementation() const
{
	TArray<FContextAction> Out = WorldActions;
	if (ItemInstance)
	{
		TArray<FContextAction> FromItem = IContextActionProvider::Execute_GetContextActions(ItemInstance);
		Out.Append(FromItem);
	}
	return Out;
}

bool AItem::ExecuteAction_Implementation(const FGameplayTag& ActionTag, FName ActionId,
                                         const FContextActionContext& Context)
{
	for (const FContextAction& A : WorldActions)
		if (A.ActionTag == ActionTag && (ActionId.IsNone() || A.ActionId == ActionId))
			return ExecuteContextAction(A, Context);

	if (ItemInstance)
		return IContextActionProvider::Execute_ExecuteAction(ItemInstance, ActionTag, ActionId, Context);

	return false;
}

AItemViewActor* AItem::GetItemViewActor() const
{
	if (IsValid(ViewActor))
		return ViewActor;
	return nullptr;
}

void AItem::Pickup(AActor* Picker)
{
	if (!Picker || !ItemInstance) return;
	if (auto* Inv = Picker->FindComponentByClass<UInventoryComponent>())
	{
		if (Inv->AddItemInstance(ItemInstance))
		{
			Destroy();
			if (IsValid(ViewActor)) ViewActor->Destroy();
		}
	}
}

void AItem::BP_OnPickup_Implementation(const FContextActionContext& Context)
{
	if (AActor* Caller = Context.Caller)
	{
		if (UInventoryComponent* Inv = Caller->FindComponentByClass<UInventoryComponent>())
		{
			if (ItemInstance)
			{
				if (Inv->CanAcceptInstance(ItemInstance))
				{
					if (Inv->AddItemInstance(ItemInstance))
					{
						Destroy();
						UE_LOG(LogTemp, Log, TEXT("Picked up item %s into inventory"), *GetName());
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Cannot pick up item %s into inventory - too heavy"), *GetName());
				}
			}
		}
	}
}

void AItem::BP_OnEquip_Implementation(const FContextActionContext& Context)
{
	if (AActor* Caller = Context.Caller)
	{
		if (UInventoryManipulatorComponent* Manip = Caller->FindComponentByClass<UInventoryManipulatorComponent>())
		{
			if (UInventoryComponent* Inv = Caller->FindComponentByClass<UInventoryComponent>())
			{
				if (ItemInstance)
				{
					if (Manip->CanHoldInHands(ItemInstance))
					{
						if (Manip->HoldInHands(ItemInstance))
						{
							Destroy();
							UE_LOG(LogTemp, Log, TEXT("Equipped item %s"), *ItemInstance->ItemDef->DisplayName.ToString());
						}
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("Too heavy to equip %s"), *ItemInstance->ItemDef->DisplayName.ToString());
					}
				}
			}
		}
	}
}

void AItem::BP_OnGrab_Implementation(const FContextActionContext& Context)
{
	if (AActor* Caller = Context.Caller)
	{
		if (UInventoryManipulatorComponent* Manip = Caller->FindComponentByClass<UInventoryManipulatorComponent>())
		{
			Manip->DragOrGrabItem(this);
			UE_LOG(LogTemp, Log, TEXT("Grabbed item %s with PhysicsHandle"), *GetName());
		}
	}
}
