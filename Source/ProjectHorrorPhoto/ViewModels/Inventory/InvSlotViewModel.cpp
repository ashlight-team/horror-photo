// Fill out your copyright notice in the Description page of Project Settings.


#include "InvSlotViewModel.h"

#include "ProjectHorrorPhoto/Components/Inventory/InventoryManipulatorComponent.h"
#include "ProjectHorrorPhoto/Components/Inventory/Types/InventoryItemInstance.h"

UInvSlotViewModel::UInvSlotViewModel()
{
	ItemIconViewModelField = NewObject<UTextureViewModel>();
	ItemCountViewModelField = NewObject<UStatViewModel>();
	SlotNumberViewModelField = NewObject<UStatViewModel>();
}

void UInvSlotViewModel::InitData(UInventoryItemInstance* ItemInstance)
{
	if (IsValid(ItemInstance))
	{
		DisplayItem(ItemInstance);
	}
}

void UInvSlotViewModel::DisplayItem(UInventoryItemInstance* ItemInstance)
{
	if (ItemInstance)
	{
		ItemIconViewModelField->SetTexture(ItemInstance->ItemDef->Icon);
		ItemCountViewModelField->SetCurrentValue(ItemInstance->StackCount);
		ItemCountViewModelField->SetbUsesMaxValue(false);
	}
	else
	{
		ItemIconViewModelField->SetTexture(nullptr);
		ItemCountViewModelField->SetCurrentValue(0);
		ItemCountViewModelField->SetMaxValue(0);
		ItemCountViewModelField->SetbUsesMaxValue(false);
	}
	SlotNumberViewModelField->SetbUsesMaxValue(false);

}
