// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Mvvm/BaseViewModel.h"
#include "ProjectHorrorPhoto/Components/Inventory/InventoryComponent.h"
#include "ProjectHorrorPhoto/ViewModels/Common/StatViewModel.h"
#include "ProjectHorrorPhoto/ViewModels/Common/TextureViewModel.h"
#include "InvSlotViewModel.generated.h"

UCLASS()
class PROJECTHORRORPHOTO_API UInvSlotViewModel : public UBaseViewModel
{
	GENERATED_BODY()

public:
	UInvSlotViewModel();

	void InitData(UInventoryItemInstance* ItemInstance);

private:
	void DisplayItem(UInventoryItemInstance* ItemInstance);

	VM_PROP_AG_AS(UTextureViewModel*, ItemIconViewModel, public, public);
	VM_PROP_AG_AS(UStatViewModel*, ItemCountViewModel, public, public);
	VM_PROP_AG_AS(UStatViewModel*, SlotNumberViewModel, public, public);
	
};
