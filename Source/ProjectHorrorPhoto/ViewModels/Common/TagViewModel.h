// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Mvvm/BaseViewModel.h"
#include "TagViewModel.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTHORRORPHOTO_API UTagViewModel : public UBaseViewModel
{
	GENERATED_BODY()
	
	VM_PROP_AG_AS(FGameplayTag, GameplayTag, public, public);
};
