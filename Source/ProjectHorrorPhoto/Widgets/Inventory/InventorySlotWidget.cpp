#include "InventorySlotWidget.h"

#include "Components/Image.h"
#include "ProjectHorrorPhoto/Widgets/ViewModelCommon/StatDisplayWidget.h"
#include "ProjectHorrorPhoto/Widgets/ViewModelCommon/TextureDisplayWidget.h"

void UInventorySlotWidget::OnViewModelChanged(UInvSlotViewModel* OldViewModel, UInvSlotViewModel* NewViewModel)
{
	if (IsValid(NewViewModel))
	{
		ItemCountText->SetViewModel(NewViewModel->GetItemCountViewModel());
		ItemImage->SetViewModel(NewViewModel->GetItemIconViewModel());
		ItemSlotText->SetViewModel(NewViewModel->GetSlotNumberViewModel());
	}
}

void UInventorySlotWidget::BindProperties()
{
	Bind(this, ViewModelType::ItemIconViewModelProperty(), &UInventorySlotWidget::OnUpdatedItemIcon);
	Bind(this, ViewModelType::ItemCountViewModelProperty(), &UInventorySlotWidget::OnUpdatedItemCount);
	Bind(this, ViewModelType::SlotNumberViewModelProperty(), &UInventorySlotWidget::OnUpdatedItemSlot);
}


void UInventorySlotWidget::OnUpdatedItemSlot(UStatViewModel* NewItemSlot)
{
	if (IsValid(NewItemSlot))
	{
		ItemSlotText->SetViewModel(NewItemSlot);
	}
}

void UInventorySlotWidget::OnUpdatedItemCount(UStatViewModel* NewItemCount)
{
	if (IsValid(NewItemCount))
	{
		ItemCountText->SetViewModel(NewItemCount);
	}
}

void UInventorySlotWidget::OnUpdatedItemIcon(UTextureViewModel* NewItemIcon)
{
	if (IsValid(NewItemIcon))
	{
		ItemImage->SetViewModel(NewItemIcon);
	}
}

