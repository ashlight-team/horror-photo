// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryHotbarViewModel.h"

#include "InvSlotViewModel.h"
#include "ProjectHorrorPhoto/Components/Inventory/InventoryManipulatorComponent.h"


void UInventoryHotbarViewModel::InitData(UInventoryManipulatorComponent* InInventoryComponent)
{
	if (IsValid(InInventoryComponent))
	{
		InventoryComponent = InInventoryComponent;
		InventoryComponent->OnHotbarUpdated.AddDynamic(this, &UInventoryHotbarViewModel::DisplayHotbar);
		DisplayHotbar();
	}
}

void UInventoryHotbarViewModel::DisplayHotbar()
{
	if (!InventoryComponent) return;

	TArray<UInvSlotViewModel*> Slots;

	for (TObjectPtr<class UInventoryItemInstance> InventoryItemInstance : InventoryComponent->HotbarItems)
	{
		UInvSlotViewModel* SlotViewModel = NewObject<UInvSlotViewModel>(this);
		if (SlotViewModel)
		{
			SlotViewModel->InitData(InventoryItemInstance);
			SlotViewModel->GetSlotNumberViewModel()->SetCurrentValue(Slots.Num() + 1);
			Slots.Add(SlotViewModel);
		}
	}
	SetHotbarSlots(Slots);

}
