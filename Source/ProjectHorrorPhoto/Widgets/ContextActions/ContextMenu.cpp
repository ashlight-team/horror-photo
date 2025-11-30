// Fill out your copyright notice in the Description page of Project Settings.


#include "ContextMenu.h"

#include "ContextActionWidget.h"
#include "FCTween.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

void UContextMenu::OnViewModelChanged(UContextMenuViewModel* OldViewModel, UContextMenuViewModel* NewViewModel)
{
	if (!NewViewModel)
		return;

	const TArray<UContextActionViewModel*> NewActions =
		NewViewModel->GetContextActionsListViewModel()->GetContextActions();
	
	bool bIsSame = (CachedActions.Num() == NewActions.Num());
	if (bIsSame)
	{
		for (int32 i = 0; i < NewActions.Num(); i++)
		{
			if (CachedActions[i] != NewActions[i])
			{
				bIsSame = false;
				break;
			}
		}
	}

	if (!bIsSame)
	{
		CachedActions = NewActions;
		DisplayMenu();  
	}
	else
	{
		ArrangeItems();  
	}
}

void UContextMenu::DisplayMenu()
{
	ActionsMenuCanvas->ClearChildren();

	for (int i = 0; i < CachedActions.Num(); i++)
	{
		auto* VM = CachedActions[i];

		auto* Widget = CreateWidget<UContextActionWidget>(this, ActionWidgetClass);
		Widget->SetViewModel(VM);
		ActionsMenuCanvas->AddChild(Widget);

		TObjectPtr<UCanvasPanelSlot> CanvasSlot = Cast<UCanvasPanelSlot>(Widget->Slot);
		CanvasSlot->SetAnchors(FAnchors(0.5f, 0.5f));
		CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		CanvasSlot->SetSize(FVector2D(ActionWidgetRadius, ActionWidgetRadius));
	}

	ArrangeItems();
}

void UContextMenu::ArrangeItems()
{
	const int32 Num = ActionsMenuCanvas->GetChildrenCount();

	for (int32 i = 0; i < Num; i++)
	{
		float ratio = float(i) / float(Num);
		float angleDeg = ratio * 360.0f + RotationOffsetDegrees;
		float angleRad = FMath::DegreesToRadians(angleDeg);

		FVector2D Offset(
			FMath::Cos(angleRad) * MenuRadius,
			FMath::Sin(angleRad) * MenuRadius
		);

		auto* Child = Cast<UContextActionWidget>(ActionsMenuCanvas->GetChildAt(i));
		if (!Child) continue;

		Child->SetRenderTransform(
			FWidgetTransform(Offset, FVector2D(1,1), FVector2D(0,0), 0)
		);
	}
}

void UContextMenu::NativeConstruct()
{
	Super::NativeConstruct();

	auto Tween = FCTween::Play(
		0.f,
		360.f,
		[&] (float t)
		{
			RotationOffsetDegrees = t;
			ArrangeItems();
		},
		5.f,
		EFCEase::Linear);
	Tween->SetLoops(-1);
}
