// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryHotbar.h"

#include "Components/HorizontalBox.h"
#include "ProjectHorrorPhoto/Components/Inventory/InventoryManipulatorComponent.h"
#include "ProjectHorrorPhoto/ViewModels/Inventory/InventoryHotbarViewModel.h"
#include "ProjectHorrorPhoto/ViewModels/Inventory/InvSlotViewModel.h"
#include "ProjectHorrorPhoto/Widgets/Inventory/InventorySlotWidget.h"

void UInventoryHotbar::OnViewModelChanged(UInventoryHotbarViewModel* OldViewModel,
	UInventoryHotbarViewModel* NewViewModel)
{
	if (NewViewModel)
	{
		OnHotbarItemsChanged(NewViewModel->GetHotbarSlots());
	}
}

void UInventoryHotbar::BindProperties()
{
	Bind(this, ViewModelType::HotbarSlotsProperty(), &ThisClass::OnHotbarItemsChanged);
}

void UInventoryHotbar::OnHotbarItemsChanged(const TArray<UInvSlotViewModel*>& NewHotbarItems)
{
	if (!HotbarBox) return;

	HotbarBox->ClearChildren();

	for (UInvSlotViewModel* NewHotbarItem : NewHotbarItems)
	{
		if (InvSlotWidgetClass)
		{
			UInventorySlotWidget* NewSlotWidget = CreateWidget<UInventorySlotWidget>(GetWorld(), InvSlotWidgetClass);
			if (NewSlotWidget)
			{
				NewSlotWidget->SetViewModel(NewHotbarItem);
				HotbarBox->AddChild(NewSlotWidget);
			}
		}
	}
}

