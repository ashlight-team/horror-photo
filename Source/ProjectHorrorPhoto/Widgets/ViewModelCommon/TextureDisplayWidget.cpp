// Fill out your copyright notice in the Description page of Project Settings.


#include "TextureDisplayWidget.h"

#include "Components/Image.h"

void UTextureDisplayWidget::OnViewModelChanged(UTextureViewModel* OldViewModel, UTextureViewModel* NewViewModel)
{
	if (IsValid(NewViewModel))
	{
		UpdateTextureDisplay(NewViewModel->GetTexture());
	}
}

void UTextureDisplayWidget::BindProperties()
{
	Bind(this, ViewModelType::TextureProperty(), &UTextureDisplayWidget::UpdateTextureDisplay);
}

void UTextureDisplayWidget::UpdateTextureDisplay(UTexture2D* NewTexture)
{
	if (ImageWidget)
	{
		if (IsValid(NewTexture))
		{
			ImageWidget->SetBrushFromTexture(NewTexture);
			ImageWidget->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			ImageWidget->SetBrushFromTexture(nullptr);
			ImageWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
