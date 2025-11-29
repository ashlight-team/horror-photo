// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "PhotoGameplayTags.generated.h"

USTRUCT()
struct FPhotoGameplayTags
{
	GENERATED_BODY()

public:
	static const FPhotoGameplayTags& Get();
	static void InitializeNativeTags();

public:
	FGameplayTag InputTag_Move;
	FGameplayTag InputTag_Look;

	FGameplayTag InputTag_Hotbar_1;
	FGameplayTag InputTag_Hotbar_2;
	FGameplayTag InputTag_Hotbar_3;
	FGameplayTag InputTag_Hotbar_4;
	FGameplayTag InputTag_Hotbar_5;
	FGameplayTag InputTag_Hotbar_6;
	FGameplayTag InputTag_Hotbar_7;
	FGameplayTag InputTag_Hotbar_8;
	FGameplayTag InputTag_Hotbar_9;
	FGameplayTag InputTag_RotateMode;
	FGameplayTag InputTag_Rotate;
	FGameplayTag InputTag_Interact;
	FGameplayTag InputTag_Sprint;
	FGameplayTag InputTag_Crouch;

	FGameplayTag Ability_Input_WhileInputActive;
	FGameplayTag Ability_Input_OnInputTriggered;

	FGameplayTag Item_Pickup;
	FGameplayTag Item_Equip;
	FGameplayTag Item_Grab;

	FGameplayTag Status_Item_Dragging;
	FGameplayTag Status_Item_Rotating;
	FGameplayTag Status_Sprinting;
	FGameplayTag Status_Crouching;

	FGameplayTag Effect_Player_MoveSetInit;

	FGameplayTag UI_Layer_Game;
	FGameplayTag UI_Layer_GameMenu;
	FGameplayTag UI_Layer_Menu;
	FGameplayTag UI_Layer_Modal;

	FGameplayTag UI_Widget_InventoryHotbar;

protected:
	void AddAllTags(UGameplayTagsManager& Manager);
	void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);

private:
	static FPhotoGameplayTags GameplayTags;
	
};
