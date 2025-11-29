// Fill out your copyright notice in the Description page of Project Settings.


#include "PhotoGameplayTags.h"

#include "GameplayTagsManager.h"

FPhotoGameplayTags FPhotoGameplayTags::GameplayTags;

const FPhotoGameplayTags& FPhotoGameplayTags::Get()
{
	return GameplayTags;
}

void FPhotoGameplayTags::InitializeNativeTags()
{
	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();

	GameplayTags.AddAllTags(Manager);

	Manager.DoneAddingNativeTags();
}


void FPhotoGameplayTags::AddAllTags(UGameplayTagsManager& Manager)
{
	AddTag(InputTag_Move, "InputTag.Move", "Move input.");
	AddTag(InputTag_Look, "InputTag.Look", "Look input.");

	AddTag(InputTag_Hotbar_1, "InputTag.Hotbar.1", "Hotbar slot 1.");
	AddTag(InputTag_Hotbar_2, "InputTag.Hotbar.2", "Hotbar slot 2.");
	AddTag(InputTag_Hotbar_3, "InputTag.Hotbar.3", "Hotbar slot 3.");
	AddTag(InputTag_Hotbar_4, "InputTag.Hotbar.4", "Hotbar slot 4.");
	AddTag(InputTag_Hotbar_5, "InputTag.Hotbar.5", "Hotbar slot 5.");
	AddTag(InputTag_Hotbar_6, "InputTag.Hotbar.6", "Hotbar slot 6.");
	AddTag(InputTag_Hotbar_7, "InputTag.Hotbar.7", "Hotbar slot 7.");
	AddTag(InputTag_Hotbar_8, "InputTag.Hotbar.8", "Hotbar slot 8.");
	AddTag(InputTag_Hotbar_9, "InputTag.Hotbar.9", "Hotbar slot 9.");
	AddTag(InputTag_RotateMode, "InputTag.RotateMode", "Toggle rotate mode.");
	AddTag(InputTag_Rotate, "InputTag.Rotate", "Rotate item.");
	AddTag(InputTag_Interact, "InputTag.Interact", "Interact with item.");
	AddTag(InputTag_Sprint, "InputTag.Sprint", "Sprint input.");
	AddTag(InputTag_Crouch, "InputTag.Crouch", "Crouch input.");

	AddTag(Ability_Input_OnInputTriggered, "Ability.Input.OnInputTriggered", "Test ability input.");
	AddTag(Ability_Input_WhileInputActive, "Ability.Input.WhileInputActive", "Test ability input.");

	AddTag(Item_Pickup, "Item.Pickup", "Pickup item into inventory.");
	AddTag(Item_Equip, "Item.Equip", "Equip item to hands.");
	AddTag(Item_Grab, "Item.Grab", "Grab item with physics handle.");

	AddTag(Status_Item_Dragging, "Status.Item.Dragging", "Status effect applied when dragging an item.");
	AddTag(Status_Item_Rotating, "Status.Item.Rotating", "Status effect applied when rotating an item.");
	AddTag(Status_Sprinting, "Status.Sprinting", "Status effect applied when sprinting.");
	AddTag(Status_Crouching, "Status.Crouching", "Status effect applied when crouching.");

	AddTag(Effect_Player_MoveSetInit, "Effect.Player.MoveSetInit", "Initial movement attribute set effect.");

	AddTag(UI_Layer_Game, "UI.Layer.Game", "Main game UI layer.");
	AddTag(UI_Layer_GameMenu, "UI.Layer.GameMenu", "In-game menu UI layer.");
	AddTag(UI_Layer_Menu, "UI.Layer.Menu", "Main menu UI layer.");
	AddTag(UI_Layer_Modal, "UI.Layer.Modal", "Modal dialog UI layer.");

	AddTag(UI_Widget_InventoryHotbar, "UI.Widget.InventoryHotbar", "Inventory hotbar widget.");
}

void FPhotoGameplayTags::AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment)
{
	OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(FName(TagName), FString(TEXT("(Native) ")) + FString(TagComment));
}
