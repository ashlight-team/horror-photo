// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItemInstance.h"

#include "ItemDefinition.h"

bool operator==(const TMap<FName, FItemCustomDataValue>& Map, const TMap<FName, FItemCustomDataValue>& RHS)
{
	if (Map.Num() != RHS.Num()) return false;

	for (const auto& Pair : Map)
	{
		const FItemCustomDataValue* OtherValue = RHS.Find(Pair.Key);
		if (!OtherValue || *OtherValue != Pair.Value)
			return false;
	}
	return true;
}

UInventoryItemInstance::UInventoryItemInstance()
{
}

float UInventoryItemInstance::GetUnitWeight() const
{
	const float Base = (ItemDef ? ItemDef->BaseStats.Weight : 0.f);
	
	float Out = Base;
	if (const FItemCustomDataValue* Ov = CustomData.Find("WeightOverride"))
	{
		Out = Ov->AsFloat(Base);
	}
	else if (const FItemCustomDataValue* D = CustomData.Find("WeightDelta"))
	{
		Out += D->AsFloat(0.f);
	}
	return FMath::Max(0.f, Out);
}

bool UInventoryItemInstance::CanStackWith(const UInventoryItemInstance* Other) const
{
	if (!Other) return false;
	if (ItemDef != Other->ItemDef) return false;
	if (CustomData != Other->CustomData) return false;
	if (!Other->ItemDef->bStackable) return false;
	
	return true;
}

bool UInventoryItemInstance::IsIdenticalTo(const UInventoryItemInstance* Other) const
{
	if (!Other) return false;
	if (ItemDef != Other->ItemDef) return false;
	if (CustomData != Other->CustomData) return false;
	
	return true;
}

void UInventoryItemInstance::InitFromInstance(const UInventoryItemInstance* Other)
{
	if (!Other) return;
	
	ItemDef = Other->ItemDef;
	StackCount = Other->StackCount;
	CustomData = Other->CustomData;
}

void UInventoryItemInstance::SetCustomDataValue(FName Key, const FItemCustomDataValue& Value)
{
	CustomData.Add(Key, Value);
}

bool UInventoryItemInstance::GetCustomDataValue(FName Key, FItemCustomDataValue& OutValue) const
{
	if (const FItemCustomDataValue* Found = CustomData.Find(Key))
	{
		OutValue = *Found;
		return true;
	}
	return false;
}

bool UInventoryItemInstance::RemoveCustomDataValue(FName Key)
{
	return CustomData.Remove(Key) > 0;
}
