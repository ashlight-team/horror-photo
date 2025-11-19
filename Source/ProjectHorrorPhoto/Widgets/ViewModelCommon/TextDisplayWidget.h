// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Mvvm/BaseView.h"
#include "ProjectHorrorPhoto/ViewModels/Common/TextViewModel.h"
#include "TextDisplayWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTHORRORPHOTO_API UTextDisplayWidget : public UCommonUserWidget, public TBaseView<UTextDisplayWidget, UTextViewModel>
{
	GENERATED_BODY()

private:
	virtual void OnViewModelChanged(UTextViewModel* OldViewModel, UTextViewModel* NewViewModel) override;

	virtual void BindProperties() override;

	void UpdateTextDisplay(const FText& NewText);

	UPROPERTY(meta=(BindWidget))
	class UCommonTextBlock* TextBlock = nullptr;
};
