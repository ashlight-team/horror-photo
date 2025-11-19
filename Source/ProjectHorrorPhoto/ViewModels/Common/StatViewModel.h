// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Mvvm/BaseViewModel.h"
#include "StatViewModel.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTHORRORPHOTO_API UStatViewModel : public UBaseViewModel
{
	GENERATED_BODY()

	VM_PROP_AG_AS(float, CurrentValue, public, public);
	VM_PROP_AG_AS(float, MaxValue, public, public);
	VM_PROP_AG_AS(bool, bUsesMaxValue, public, public);
};
