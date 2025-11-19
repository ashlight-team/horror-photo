// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"


#include "InventoryComponent.h"
#include "Engine/World.h"
#include "Types/InventoryItemInstance.h"
#include "Types/Item.h"
#include "Types/ItemDefinition.h"

float UInventoryComponent::GetCurrentWeight() const
{
	float Sum = 0.f;
	for (const UInventoryItemInstance* It : Items)
	{
		if (!It) continue;
		Sum += It->GetUnitWeight() * FMath::Max(1, It->StackCount);
	}
	return Sum;
}

bool UInventoryComponent::CanAcceptInstance(const UInventoryItemInstance* Instance) const
{
	if (!Instance) return false;
	const float add = Instance->GetUnitWeight() * FMath::Max(1, Instance->StackCount);
	return (GetCurrentWeight() + add) <= MaxInventoryWeight + KINDA_SMALL_NUMBER;
}

bool UInventoryComponent::CanAccept(UItemDefinition* Def, int32 Amount) const
{
	if (!Def || Amount <= 0) return false;
	const float add = Def->BaseStats.Weight * Amount;
	return (GetCurrentWeight() + add) <= MaxInventoryWeight + KINDA_SMALL_NUMBER;
}

bool UInventoryComponent::AddItem(UItemDefinition* Def, int32 Amount)
{
	if (!Def || Amount <= 0) return false;
	if (!CanAccept(Def, Amount)) return false;

	UInventoryItemInstance* NewInst = NewObject<UInventoryItemInstance>(this);
	NewInst->ItemDef = Def;
	NewInst->StackCount = Amount;
	
	return AddItemInstance(NewInst);
}

static inline bool CanMergeStacks(const UInventoryItemInstance* A, const UInventoryItemInstance* B)
{
	return A && B && A->IsIdenticalTo(B);
}

UInventoryItemInstance* UInventoryComponent::AddOrGetItem(UItemDefinition* Def, int32 Amount)
{
	if (!Def || Amount <= 0) return nullptr;

	for (UInventoryItemInstance* Existing : Items)
	{
		if (Existing)
		{
			UInventoryItemInstance* Temp = NewObject<UInventoryItemInstance>(this);
			Temp->ItemDef = Def; 
			Temp->StackCount = Amount;
			Temp->InitFromInstance(Existing); 

			if (CanMergeStacks(Existing, Temp))
			{
				if (!CanAccept(Def, Amount)) return nullptr;
				Existing->StackCount += Amount;
				return Existing;
			}
		}
	}

	
	if (!CanAccept(Def, Amount)) return nullptr;
	UInventoryItemInstance* NewInstance = NewObject<UInventoryItemInstance>(this);
	NewInstance->ItemDef = Def;
	NewInstance->StackCount = Amount;
	Items.Add(NewInstance);
	return NewInstance;
}

bool UInventoryComponent::ConsumeOne(class UInventoryItemInstance* Instance)
{
	if (!Instance) return false;

	if (Instance->StackCount > 1)
	{
		Instance->StackCount--;
	}
	else
	{
		Items.Remove(Instance);
	}
	return true;
}

bool UInventoryComponent::AddItemInstance(UInventoryItemInstance* Instance)
{
	if (!Instance) return false;

	for (UInventoryItemInstance* Item : Items)
	{
		if (CanMergeStacks(Item, Instance))
		{
			const float add = Instance->GetUnitWeight() * Instance->StackCount;
			if ((GetCurrentWeight() + add) > MaxInventoryWeight + KINDA_SMALL_NUMBER)
				return false;
			
			Item->StackCount += Instance->StackCount;
			return true;
		}
	}
	
	if (!CanAcceptInstance(Instance)) return false;
	Items.Add(Instance);
	return true;
}

bool UInventoryComponent::RemoveItemInstance(UInventoryItemInstance* Instance, int32 Amount)
{
	if (!Instance || Amount <= 0) return false;

	for (int32 i = 0; i < Items.Num(); ++i)
	{
		UInventoryItemInstance* Item = Items[i];
		if (Item && Item->IsIdenticalTo(Instance))
		{
			if (Item->StackCount > Amount)
			{
				Item->StackCount -= Amount;
				return true;
			}
			else
			{
				Items.RemoveAt(i);
				return true;
			}
		}
	}
	return false;
}



bool UInventoryComponent::DropItemInstance(UInventoryItemInstance* Instance, const FTransform& Xform, bool bDropAll)
{
	if (!Instance || !Instance->ItemDef) return false;

	UClass* SpawnClass = Instance->ItemDef->OverrideActorClass
		                     ? Instance->ItemDef->OverrideActorClass.Get()
		                     : AItem::StaticClass();

	if (bDropAll)
	{
		int32 CountToDrop = Instance->StackCount;

		for (int32 i = 0; i < CountToDrop; ++i)
		{
			AItem* Spawned = GetWorld()->SpawnActorDeferred<AItem>(SpawnClass, Xform, GetOwner());
			if (!Spawned) return false;

			UInventoryItemInstance* DroppedInst = NewObject<UInventoryItemInstance>(this);
			DroppedInst->InitFromInstance(Instance);
			DroppedInst->StackCount = 1;


			Spawned->InitializeFromInstance(DroppedInst);
			Spawned->FinishSpawning(Xform);
		}
		RemoveItemInstance(Instance, CountToDrop);
	}
	else
	{
		AItem* Spawned = GetWorld()->SpawnActorDeferred<AItem>(SpawnClass, Xform, GetOwner());
		if (!Spawned) return false;

		UInventoryItemInstance* DroppedInst = NewObject<UInventoryItemInstance>(this);
		DroppedInst->InitFromInstance(Instance);
		DroppedInst->StackCount = 1;

		Spawned->InitializeFromInstance(DroppedInst);
		Spawned->FinishSpawning(Xform);
		
		RemoveItemInstance(DroppedInst, 1);
	}

	return true;
}
