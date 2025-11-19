// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Mvvm/BaseView.h"
#include "ProjectHorrorPhoto/ViewModels/Common/StatViewModel.h"
#include "StatDisplayWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTHORRORPHOTO_API UStatDisplayWidget : public UCommonUserWidget, public TBaseView<UStatDisplayWidget, UStatViewModel>
{
	GENERATED_BODY()

private:
	virtual void OnViewModelChanged(UStatViewModel* OldViewModel, UStatViewModel* NewViewModel) override;

	virtual void BindProperties() override;

	void UpdateStatDisplay(float NewCurrentValue);

	UPROPERTY(meta=(BindWidget))
	class UCommonTextBlock* CurrentValueText = nullptr;
};
