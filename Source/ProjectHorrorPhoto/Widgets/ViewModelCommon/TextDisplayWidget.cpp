// Fill out your copyright notice in the Description page of Project Settings.


#include "TextDisplayWidget.h"

#include "CommonTextBlock.h"
#include "Components/TextBlock.h"

void UTextDisplayWidget::OnViewModelChanged(UTextViewModel* OldViewModel, UTextViewModel* NewViewModel)
{
	if (IsValid(NewViewModel))
	{
		UpdateTextDisplay(NewViewModel->GetText());
	}
}

void UTextDisplayWidget::BindProperties()
{
	Bind(this, ViewModelType::TextProperty(), &UTextDisplayWidget::UpdateTextDisplay);
}

void UTextDisplayWidget::UpdateTextDisplay(const FText& NewText)
{
	if (TextBlock)
	{
		TextBlock->SetText(NewText);
	}
}
