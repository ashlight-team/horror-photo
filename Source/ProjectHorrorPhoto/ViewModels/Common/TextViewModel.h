// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Mvvm/BaseViewModel.h"

#include "TextViewModel.generated.h"

UCLASS()
class PROJECTHORRORPHOTO_API UTextViewModel : public UBaseViewModel
{
	GENERATED_BODY()

	VM_PROP_AG_AS(FText, Text, public, public);
};
