// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Mvvm/BaseView.h"
#include "ProjectHorrorPhoto/ViewModels/Inventory/InventoryHotbarViewModel.h"
#include "InventoryHotbar.generated.h"

UCLASS()
class PROJECTHORRORPHOTO_API UInventoryHotbar : public UCommonUserWidget, public TBaseView<UInventoryHotbar, UInventoryHotbarViewModel>
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn=true))
	class UInventoryManipulatorComponent* InventoryManipulatorComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class UInventorySlotWidget> InvSlotWidgetClass;

private:
	virtual void OnViewModelChanged(UInventoryHotbarViewModel* OldViewModel, UInventoryHotbarViewModel* NewViewModel) override;

	virtual void BindProperties() override;

	UPROPERTY(meta=(BindWidget))
	class UHorizontalBox* HotbarBox = nullptr;
	
	void OnHotbarItemsChanged(const TArray<class UInvSlotViewModel*>& NewHotbarItems);
};
