// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Mvvm/BaseView.h"
#include "ProjectHorrorPhoto/ViewModels/ContextAction/ContextActionViewModel.h"
#include "ContextActionWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTHORRORPHOTO_API UContextActionWidget : public UCommonUserWidget, public TBaseView<UContextActionWidget, UContextActionViewModel>
{
	GENERATED_BODY()

private:
	virtual void OnViewModelChanged(UContextActionViewModel* OldViewModel, UContextActionViewModel* NewViewModel) override;
	
	UPROPERTY(meta=(BindWidget))
	class UTextureDisplayWidget* ActionImage = nullptr;

	UPROPERTY(meta=(BindWidget))
	class UTextDisplayWidget* ActionDisplayName = nullptr;

	UPROPERTY(meta=(BindWidget))
	class UButton* ActionButton = nullptr;

	UFUNCTION()
	void HandleActionButtonClicked();
};
