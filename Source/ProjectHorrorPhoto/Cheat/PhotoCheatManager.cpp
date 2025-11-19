


#include "PhotoCheatManager.h"

#include "ProjectHorrorPhoto/Components/Inventory/InventoryComponent.h"
#include "ProjectHorrorPhoto/Components/Inventory/InventoryManipulatorComponent.h"
#include "ProjectHorrorPhoto/Components/Inventory/Types/InventoryItemInstance.h"
#include "ProjectHorrorPhoto/Components/Inventory/Types/Item.h"
#include "ProjectHorrorPhoto/Components/Inventory/Types/ItemDefinition.h"
#include "ProjectHorrorPhoto/Player/PhotoPlayerCharacter.h"

void UPhotoCheatManager::DebugPrintInventory()
{
    if (auto* PC = GetOuterAPlayerController())
    {
        if (auto* Char = Cast<APhotoPlayerCharacter>(PC->GetPawn()))
        {
            if (auto* Inv = Char->FindComponentByClass<UInventoryComponent>())
            {
                for (int32 i=0; i<Inv->Items.Num(); i++)
                {
                    if (UInventoryItemInstance* Inst = Inv->Items[i])
                    {
                        UE_LOG(LogTemp, Warning, TEXT("[%d] %s x%d"),
                            i,
                            *Inst->ItemDef->DisplayName.ToString(),
                            Inst->StackCount);
                    }
                }
            }
        }
    }
}

void UPhotoCheatManager::TakeItem()
{
    if (auto* PC = GetOuterAPlayerController())
    {
        if (auto* Char = Cast<APhotoPlayerCharacter>(PC->GetPawn()))
        {
            if (auto* Inv = Char->FindComponentByClass<UInventoryComponent>())
            {
                if (auto* Manip = Char->FindComponentByClass<UInventoryManipulatorComponent>())
                {
                    if (Inv->Items.Num() > 0)
                    {
                    	Manip->TakeToHands(Inv->Items[0]);
                    }
                }
            }
        }
    }
}

void UPhotoCheatManager::ItemExec(FString TagStr)
{
	FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(*TagStr));

	if (auto* PC = GetOuterAPlayerController())
	{
		if (auto* Char = Cast<APhotoPlayerCharacter>(PC->GetPawn()))
		{
			if (auto* Manip = Char->FindComponentByClass<UInventoryManipulatorComponent>())
			{
				if (Manip->HeldItem)
				{
					FContextActionContext Ctx;
					Ctx.Caller = Char;
					Ctx.SourceItem = Manip->HeldItem;

					Manip->HeldItem->ExecuteAction(Tag, NAME_None, Ctx);
					UE_LOG(LogTemp, Warning, TEXT("Executed %s on HeldItem"), *Tag.ToString());
					return;
				}

				FVector Loc; FRotator Rot;
				PC->GetPlayerViewPoint(Loc, Rot);
				FVector End = Loc + Rot.Vector() * 500.f;

				FHitResult Hit;
				if (Char->GetWorld()->LineTraceSingleByChannel(Hit, Loc, End, ECC_Visibility))
				{
					if (AActor* HitActor = Hit.GetActor())
					{
						if (HitActor->GetClass()->ImplementsInterface(UContextActionProvider::StaticClass()))
						{
							FContextActionContext Ctx;
							Ctx.Caller = Char;
							Ctx.SourceActor = HitActor;

							IContextActionProvider::Execute_ExecuteAction(HitActor, Tag, NAME_None, Ctx);
							UE_LOG(LogTemp, Warning, TEXT("Executed %s on World Actor %s"), *Tag.ToString(), *HitActor->GetName());
							return;
						}
					}
				}
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("No target for action %s"), *Tag.ToString());
}

void UPhotoCheatManager::HeldItemExec(FString TagStr)
{
	FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(*TagStr));

	if (auto* PC = GetOuterAPlayerController())
	{
		if (auto* Char = Cast<APhotoPlayerCharacter>(PC->GetPawn()))
		{
			if (auto* Manip = Char->FindComponentByClass<UInventoryManipulatorComponent>())
			{
				if (Manip->HeldItem)
				{
					FContextActionContext Ctx;
					Ctx.Caller = Char;
					Ctx.SourceItem = Manip->HeldItem;

					Manip->HeldItem->ExecuteAction(Tag, NAME_None, Ctx);
					UE_LOG(LogTemp, Warning, TEXT("Executed %s on HeldItem"), *Tag.ToString());
					return;
				}
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("No HeldItem for action %s"), *Tag.ToString());
}

void UPhotoCheatManager::AddEquippedToHotbar(int32 HotbarIndex)
{
	if (auto* PC = GetOuterAPlayerController())
	{
		if (auto* Char = Cast<APhotoPlayerCharacter>(PC->GetPawn()))
		{
			if (auto* Manip = Char->FindComponentByClass<UInventoryManipulatorComponent>())
			{
				if (Manip->EquippedItem)
				{
					if (Manip->AssignHotbarSlot(HotbarIndex, Manip->EquippedItem))
					{
						UE_LOG(LogTemp, Warning, TEXT("Added EquippedItem to hotbar slot %d"), HotbarIndex);
						Manip->EquippedItem = nullptr;
						Manip->DestroyEquippedVisual();
						return;
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("Failed to add EquippedItem to hotbar slot %d"), HotbarIndex);
						return;
					}
				}
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("No EquippedItem to add to hotbar"));
}

void UPhotoCheatManager::ActiveHotbarSlot(int32 HotbarIndex)
{
	if (auto* PC = GetOuterAPlayerController())
	{
		if (auto* Char = Cast<APhotoPlayerCharacter>(PC->GetPawn()))
		{
			if (auto* Manip = Char->FindComponentByClass<UInventoryManipulatorComponent>())
			{
				if (Manip->ActivateHotbarSlot(HotbarIndex))
				{
					UE_LOG(LogTemp, Warning, TEXT("Activated hotbar slot %d"), HotbarIndex);
					return;
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Failed to activate hotbar slot %d"), HotbarIndex);
					return;
				}
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("No InventoryManipulatorComponent to activate hotbar slot"));
}

void UPhotoCheatManager::ItemDrop()
{
	if (auto* PC = GetOuterAPlayerController())
	{
		if (auto* Char = Cast<APhotoPlayerCharacter>(PC->GetPawn()))
		{
			if (auto* Manip = Char->FindComponentByClass<UInventoryManipulatorComponent>())
			{
				Manip->DropGrabbedItem();
				UE_LOG(LogTemp, Warning, TEXT("Unequipped item"));
			}
		}
	}
}

void UPhotoCheatManager::ItemThrow()
{
	if (auto* PC = GetOuterAPlayerController())
	{
		if (auto* Char = Cast<APhotoPlayerCharacter>(PC->GetPawn()))
		{
			if (auto* Manip = Char->FindComponentByClass<UInventoryManipulatorComponent>())
			{
				Manip->ThrowGrabbedItem(2000.f);
				UE_LOG(LogTemp, Warning, TEXT("Thrown item"));
			}
		}
	}
}

void UPhotoCheatManager::EquippedDrop()
{
	if (auto* PC = GetOuterAPlayerController())
	{
		if (auto* Char = Cast<APhotoPlayerCharacter>(PC->GetPawn()))
		{
			if (auto* Manip = Char->FindComponentByClass<UInventoryManipulatorComponent>())
			{
				if (Manip->EquippedItem)
				{
					FVector Loc; FRotator Rot;
					PC->GetPlayerViewPoint(Loc, Rot);
					FVector End = Loc + Rot.Vector() * 200.f;

					FTransform SpawnXform(Rot, End);
					if (Manip->DropEquippedItem(SpawnXform))
					{
						UE_LOG(LogTemp, Warning, TEXT("Dropped equipped item"));
					}
				}
			}
		}
	}
}

void UPhotoCheatManager::EquippedThrow()
{
	if (auto* PC = GetOuterAPlayerController())
	{
		if (auto* Char = Cast<APhotoPlayerCharacter>(PC->GetPawn()))
		{
			if (auto* Manip = Char->FindComponentByClass<UInventoryManipulatorComponent>())
			{
				Manip->ThrowEquippedItem(2000.f);
				UE_LOG(LogTemp, Warning, TEXT("Thrown equipped item"));
			}
		}
	}
}

void UPhotoCheatManager::UntakeItem()
{
	if (auto* PC = GetOuterAPlayerController())
	{
		if (auto* Char = Cast<APhotoPlayerCharacter>(PC->GetPawn()))
		{
			if (auto* Manip = Char->FindComponentByClass<UInventoryManipulatorComponent>())
			{
				Manip->PutFromHandsToInventory();
				UE_LOG(LogTemp, Warning, TEXT("Unequipped item"));
			}
		}
	}
}

void UPhotoCheatManager::GrabEquipped()
{
	if (auto* PC = GetOuterAPlayerController())
	{
		if (auto* Char = Cast<APhotoPlayerCharacter>(PC->GetPawn()))
		{
			if (auto* Manip = Char->FindComponentByClass<UInventoryManipulatorComponent>())
			{
				if (Manip->GrabEquipped())
				{
					UE_LOG(LogTemp, Warning, TEXT("Grabbed equipped item"));
				}
			}
		}
	}
}

void UPhotoCheatManager::EquipGrabbed()
{
	if (auto* PC = GetOuterAPlayerController())
	{
		if (auto* Char = Cast<APhotoPlayerCharacter>(PC->GetPawn()))
		{
			if (auto* Manip = Char->FindComponentByClass<UInventoryManipulatorComponent>())
			{
				if (Manip->EquipGrabbed())
				{
					UE_LOG(LogTemp, Warning, TEXT("Equipped grabbed item"));
				}
			}
		}
	}
}


void UPhotoCheatManager::GrabNearest()
{
	if (auto* PC = GetOuterAPlayerController())
	{
		if (auto* Char = Cast<APhotoPlayerCharacter>(PC->GetPawn()))
		{
			if (auto* Manip = Char->FindComponentByClass<UInventoryManipulatorComponent>())
			{
				FVector Loc; FRotator Rot;
				PC->GetPlayerViewPoint(Loc, Rot);
				FVector End = Loc + Rot.Vector()*500.f;

				FHitResult Hit;
				if (Char->GetWorld()->LineTraceSingleByChannel(Hit, Loc, End, ECC_Visibility))
				{
					if (AItem* Item = Cast<AItem>(Hit.GetActor()))
					{
						Manip->DragOrGrabItem(Item);
						UE_LOG(LogTemp, Warning, TEXT("Grabbed %s"), *Item->GetName());
					}
				}
			}
		}
	}
}

void UPhotoCheatManager::Ragdollify()
{
	if (auto* PC = GetOuterAPlayerController())
	{
		if (auto* Char = Cast<APhotoPlayerCharacter>(PC->GetPawn()))
		{
			Char->ToggleRagdoll();
			UE_LOG(LogTemp, Warning, TEXT("Ragdollified"));
		}
	}
}
