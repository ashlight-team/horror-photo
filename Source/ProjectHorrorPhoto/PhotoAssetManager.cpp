// Fill out your copyright notice in the Description page of Project Settings.


#include "PhotoAssetManager.h"

#include "AbilitySystem/PhotoGameplayTags.h"

void UPhotoAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	FPhotoGameplayTags::InitializeNativeTags();
}
