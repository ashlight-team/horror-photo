// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "PhotoCheatManager.generated.h"

UCLASS()
class PROJECTHORRORPHOTO_API UPhotoCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
	UFUNCTION(exec)
	void DebugPrintInventory();

	UFUNCTION(exec)
	void TakeItem();

	UFUNCTION(exec)
	void ItemExec(FString TagStr);

	UFUNCTION(exec)
	void HeldItemExec(FString TagStr);

	UFUNCTION(exec)
	void AddEquippedToHotbar(int32 HotbarIndex);

	UFUNCTION(exec)
	void ActiveHotbarSlot(int32 HotbarIndex);

	UFUNCTION(exec)
	void ItemDrop();

	UFUNCTION(exec)
	void ItemThrow();

	UFUNCTION(exec)
	void EquippedDrop();

	UFUNCTION(exec)
	void EquippedThrow();

	UFUNCTION(exec)
	void UntakeItem();

	UFUNCTION(exec)
	void GrabEquipped();

	UFUNCTION(exec)
	void EquipGrabbed();

	UFUNCTION(exec)
	void GrabNearest();

	UFUNCTION(exec)
	void Ragdollify();
};
