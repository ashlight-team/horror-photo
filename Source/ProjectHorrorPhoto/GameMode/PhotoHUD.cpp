// Fill out your copyright notice in the Description page of Project Settings.


#include "PhotoHUD.h"

#include "CommonUserWidget.h"
#include "Blueprint/UserWidget.h"
#include "ProjectHorrorPhoto/Components/Inventory/InventoryComponent.h"
#include "ProjectHorrorPhoto/Components/Inventory/InventoryManipulatorComponent.h"
#include "ProjectHorrorPhoto/Components/Inventory/Interfaces/ContextActionProvider.h"
#include "ProjectHorrorPhoto/Components/Inventory/Types/InventoryItemInstance.h"
#include "ProjectHorrorPhoto/Components/Inventory/Types/Item.h"
#include "ProjectHorrorPhoto/Components/Inventory/Types/ItemDefinition.h"
#include "ProjectHorrorPhoto/Player/PhotoPlayerCharacter.h"

void APhotoHUD::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	APlayerController* PC = GetOwningPlayerController();
	if (!PC) return;

	FVector Loc; FRotator Rot;
	PC->GetPlayerViewPoint(Loc, Rot);
	FVector End = Loc + Rot.Vector() * 500.f;

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, Loc, End, ECC_Visibility))
	{
		DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 10.f, FColor::Red, false, 0.f);
		if (AActor* HitActor = Hit.GetActor())
		{
			if (HitActor->GetClass()->ImplementsInterface(UContextActionProvider::StaticClass()))
			{
				auto Actions = IContextActionProvider::Execute_GetContextActions(HitActor);
				FString ActionsStr;

				for (const auto& Action : Actions)
				{
					ActionsStr += FString::Printf(TEXT(" - %s (%s)\n"), *Action.DisplayName.ToString(), *Action.ActionTag.ToString());
				}

				GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::Green, FString::Printf(TEXT("Hit: %s"), *HitActor->GetName()));
				if (!ActionsStr.IsEmpty())
				{
					GEngine->AddOnScreenDebugMessage(10, 0.f, FColor::White, FString::Printf(TEXT("Actions:\n%s"), *ActionsStr));
				}
			}
			
			


			UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(HitActor->FindComponentByClass<UStaticMeshComponent>());
			if (MeshComponent && MeshComponent->IsSimulatingPhysics())			
			{
				float Mass = MeshComponent->GetMass();
				FVector Velocity = MeshComponent->GetPhysicsLinearVelocity();
				float LinearDamping = MeshComponent->GetLinearDamping();
				float AngularDamping = MeshComponent->GetAngularDamping();

				FString PhysStr = FString::Printf(TEXT("Mass: %.1f kg\nVelocity: X=%.1f, Y= %.1f, Z=%.1f cm/s\nLinear Damping: %.3f\nAngular Damping: %.3f"),
												 Mass,
												 Velocity.X,
												 Velocity.Y,
												 Velocity.Z,
												 LinearDamping,
												 AngularDamping);

				DrawDebugString(GetWorld(), Hit.ImpactPoint + FVector(0,0,20), PhysStr, nullptr, FColor::White, 0.f, true);
			}
			
		}
	}

	PrintInventoryDebug();
	PrintHotbarDebug();
}

void APhotoHUD::PrintInventoryDebug()
{
	if (APlayerController* PC = GetOwningPlayerController())
	{
		if (APawn* P = PC->GetPawn())
		{
			if (auto* Inv = P->FindComponentByClass<UInventoryComponent>())
			{
				FString InvStr;
				InvStr += TEXT("Inventory:\n");
				InvStr += FString::Printf(TEXT(" Total Items: %d\n"), Inv->Items.Num());
				InvStr += FString::Printf(TEXT(" Current Weight: %.2f / %.2f\n"), Inv->GetCurrentWeight(), Inv->MaxInventoryWeight);
				
				for (const auto& Item : Inv->Items)
				{
					InvStr += FString::Printf(TEXT(" - %s x%d\n"), *Item->ItemDef->DisplayName.ToString(), Item->StackCount);
				}

				GEngine->AddOnScreenDebugMessage(2, 5.0f, FColor::Yellow, InvStr);

				if (auto* Manip = P->FindComponentByClass<UInventoryManipulatorComponent>())
				{
					if (Manip->EquippedItem)
					{
						FString EquippedStr = FString::Printf(TEXT("Equipped: %s\n"), *Manip->EquippedItem->ItemDef->DisplayName.ToString());
						EquippedStr += FString::Printf(TEXT("Weight in hand: %.2f"), Manip->GetEquippedWeight());
						GEngine->AddOnScreenDebugMessage(3, 5.0f, FColor::Cyan, EquippedStr);
					}
					else
					{
						GEngine->AddOnScreenDebugMessage(3, 5.0f, FColor::Cyan, TEXT("Equipped: None"));
					}
				}

				if (auto* Manip = P->FindComponentByClass<UInventoryManipulatorComponent>())
				{
					if (Manip->GrabbedItem)
					{
						FString GrabbedStr = FString::Printf(TEXT("Grabbed: %s"), *Manip->GrabbedItem->GetName());
						GEngine->AddOnScreenDebugMessage(4, 5.0f, FColor::Magenta, GrabbedStr);
					}
					else
					{
						GEngine->AddOnScreenDebugMessage(4, 5.0f, FColor::Magenta, TEXT("Grabbed: None"));
					}
				}
			}
		}
	}
}

void APhotoHUD::PrintHotbarDebug()
{
	if (APlayerController* PC = GetOwningPlayerController())
	{
		if (APawn* P = PC->GetPawn())
		{
			if (auto* Manip = P->FindComponentByClass<UInventoryManipulatorComponent>())
			{
				FString HotbarStr;
				HotbarStr += TEXT("Hotbar:\n");
				HotbarStr += FString::Printf(TEXT(" Max Slots: %d\n"), Manip->MaxHotbarSlots);
				HotbarStr += FString::Printf(TEXT(" Current Weight: %.2f / %.2f\n"), Manip->GetHotbarWeight(), Manip->MaxHotbarWeight);
				for (int32 i = 0; i < Manip->HotbarItems.Num(); i++)
				{
					if (Manip->HotbarItems[i])
					{
						HotbarStr += FString::Printf(TEXT(" [%d] %s x%d\n"), i, *Manip->HotbarItems[i]->ItemDef->DisplayName.ToString(), Manip->HotbarItems[i]->StackCount);
					}
					else
					{
						HotbarStr += FString::Printf(TEXT(" [%d] Empty\n"), i);
					}
				}

				GEngine->AddOnScreenDebugMessage(5, 5.0f, FColor::Orange, HotbarStr);
			}
		}
	}
}

void APhotoHUD::BeginPlay()
{
	Super::BeginPlay();

	if (BasicFrontendWidgetClass)
	{
		if (APlayerController* PC = GetOwningPlayerController())
		{
			if (UCommonUserWidget* W = CreateWidget<UCommonUserWidget>(PC, BasicFrontendWidgetClass))
			{
				W->AddToViewport(0);
			}
		}
	}
}
