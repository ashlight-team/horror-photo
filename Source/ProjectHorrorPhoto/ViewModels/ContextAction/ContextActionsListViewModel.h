// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ContextActionViewModel.h"
#include "Mvvm/BaseViewModel.h"
#include "ContextActionsListViewModel.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTHORRORPHOTO_API UContextActionsListViewModel : public UBaseViewModel
{
	GENERATED_BODY()

	VM_PROP_AG_AS((const TArray<UContextActionViewModel*>&), ContextActions, public, public);
};
