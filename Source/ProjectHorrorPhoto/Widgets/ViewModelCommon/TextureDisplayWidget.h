// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Mvvm/BaseView.h"
#include "ProjectHorrorPhoto/ViewModels/Common/TextureViewModel.h"
#include "TextureDisplayWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTHORRORPHOTO_API UTextureDisplayWidget : public UCommonUserWidget, public TBaseView<UTextureDisplayWidget, UTextureViewModel>
{
	GENERATED_BODY()

private:
	virtual void OnViewModelChanged(UTextureViewModel* OldViewModel, UTextureViewModel* NewViewModel) override;

	virtual void BindProperties() override;

	void UpdateTextureDisplay(UTexture2D* NewTexture);

	UPROPERTY(meta=(BindWidget))
	class UImage* ImageWidget = nullptr;
};
