// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "PhotoAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTHORRORPHOTO_API UPhotoAssetManager : public UAssetManager
{
	GENERATED_BODY()

	virtual void StartInitialLoading() override;
};
