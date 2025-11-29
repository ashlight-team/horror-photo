// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ContextActionsListViewModel.h"
#include "Mvvm/BaseViewModel.h"
#include "ProjectHorrorPhoto/Components/Inventory/Interfaces/ContextActionProvider.h"
#include "ContextMenuViewModel.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTHORRORPHOTO_API UContextMenuViewModel : public UBaseViewModel
{
	GENERATED_BODY()

public:
	UContextMenuViewModel();

	void InitData(AActor* HitActor, AActor* CallerActor);
private:
	void DisplayContextMenu(AActor* HitActor, AActor* CallerActor);
	
	VM_PROP_AG_AS(UContextActionsListViewModel*, ContextActionsListViewModel, public, public);
};
