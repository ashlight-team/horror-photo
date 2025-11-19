// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Mvvm/BaseViewModel.h"
#include "TextureViewModel.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTHORRORPHOTO_API UTextureViewModel : public UBaseViewModel
{
	GENERATED_BODY()

	VM_PROP_AG_AS(UTexture2D*, Texture, public, public);
};
