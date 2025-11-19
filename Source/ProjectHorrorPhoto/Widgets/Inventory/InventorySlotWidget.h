// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Mvvm/BaseView.h"
#include "ProjectHorrorPhoto/ViewModels/Inventory/InvSlotViewModel.h"
#include "InventorySlotWidget.generated.h"


UCLASS()
class PROJECTHORRORPHOTO_API UInventorySlotWidget : public UCommonUserWidget, public TBaseView<UInventorySlotWidget, UInvSlotViewModel>
{
	GENERATED_BODY()

private:
	virtual void OnViewModelChanged(UInvSlotViewModel* OldViewModel, UInvSlotViewModel* NewViewModel) override;

	virtual void BindProperties() override;
	
	UPROPERTY(meta=(BindWidget))
	class UTextureDisplayWidget* ItemImage = nullptr;

	UPROPERTY(meta=(BindWidget))
	class UStatDisplayWidget* ItemCountText = nullptr;

	UPROPERTY(meta=(BindWidget))
	class UStatDisplayWidget* ItemSlotText = nullptr;

	
	void OnUpdatedItemSlot(UStatViewModel* NewItemSlot);
	void OnUpdatedItemCount(UStatViewModel* NewItemCount);
	void OnUpdatedItemIcon(UTextureViewModel* NewItemIcon);
};
