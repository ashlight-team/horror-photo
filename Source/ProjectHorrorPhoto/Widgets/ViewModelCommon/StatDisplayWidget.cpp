// Fill out your copyright notice in the Description page of Project Settings.


#include "StatDisplayWidget.h"

#include "CommonTextBlock.h"
#include "Components/TextBlock.h"

void UStatDisplayWidget::OnViewModelChanged(UStatViewModel* OldViewModel, UStatViewModel* NewViewModel)
{
	if (IsValid(NewViewModel))
	{
		UpdateStatDisplay(NewViewModel->GetCurrentValue());
	}
}

void UStatDisplayWidget::BindProperties()
{
	Bind(this, ViewModelType::CurrentValueProperty(), &UStatDisplayWidget::UpdateStatDisplay);
}

void UStatDisplayWidget::UpdateStatDisplay(float NewCurrentValue)
{
	if (IsValid(CurrentValueText))
	{
		FString	NewText;
		NewText = FString::SanitizeFloat(NewCurrentValue,0);
		if (GetViewModel()->GetbUsesMaxValue())
		{
			NewText.Append(TEXT("/"));
			NewText.Append(FString::SanitizeFloat(GetViewModel()->GetMaxValue(),0));
		}

		CurrentValueText->SetText(FText::FromString(NewText));
	}
}
