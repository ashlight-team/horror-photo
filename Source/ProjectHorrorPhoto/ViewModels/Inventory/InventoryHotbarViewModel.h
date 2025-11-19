// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InvSlotViewModel.h"
#include "Mvvm/BaseViewModel.h"
#include "InventoryHotbarViewModel.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTHORRORPHOTO_API UInventoryHotbarViewModel : public UBaseViewModel
{
	GENERATED_BODY()

public:
	void InitData(class UInventoryManipulatorComponent* InInventoryComponent);
	
private:
	UFUNCTION()
	void DisplayHotbar();
	
	VM_PROP_AG_AS((const TArray<UInvSlotViewModel*>&), HotbarSlots, public, public);

	UPROPERTY()
	class UInventoryManipulatorComponent* InventoryComponent = nullptr;
};
