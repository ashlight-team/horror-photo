// Fill out your copyright notice in the Description page of Project Settings.


#include "ContextMenuViewModel.h"

UContextMenuViewModel::UContextMenuViewModel()
{
	ContextActionsListViewModelField = NewObject<UContextActionsListViewModel>();
}

void UContextMenuViewModel::InitData(AActor* HitActor, AActor* CallerActor)
{
	if (IsValid(HitActor) || IsValid(CallerActor))
	{
		DisplayContextMenu(HitActor, CallerActor);
	}
}

void UContextMenuViewModel::DisplayContextMenu(AActor* HitActor, AActor* CallerActor)
{
	TArray<FContextAction> Actions = IContextActionProvider::Execute_GetContextActions(HitActor);
	const TArray<FContextAction>* ActionsPtr = &Actions;

	TArray<UContextActionViewModel*> ActionViewModels;

	for (const FContextAction& ContextAction : *ActionsPtr)
	{
		UContextActionViewModel* ActionViewModel = NewObject<UContextActionViewModel>(this);
		if (ActionViewModel)
		{
			ActionViewModel->InitData(ContextAction, HitActor, CallerActor);
			ActionViewModels.Add(ActionViewModel);
		}
	}

	ContextActionsListViewModelField->SetContextActions(ActionViewModels);
}
