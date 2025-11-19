// Fill out your copyright notice in the Description page of Project Settings.


#include "ExtensionWidget.h"

#include "CommonLayersSubsystem.h"

UExtensionWidget::UExtensionWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

TSharedRef<SWidget> UExtensionWidget::RebuildWidget()
{
	if (!IsDesignTime() && ExtensionLayerTag.IsValid())
	{
		UGameInstance* GameInstance = GetGameInstance();
		if (GameInstance)
		{
			UCommonLayersSubsystem* LayersSubsystem = GameInstance->GetSubsystem<UCommonLayersSubsystem>();
			if (LayersSubsystem)
			{
				LayersSubsystem->RegisterExtensionLayer(ExtensionLayerTag, this);
			}
		}
	}

	if (IsDesignTime())
	{
		auto GetExtensionPointText = [this]()
		{
			return FText::FromString(FString::Printf(TEXT("Extension Point: %s"), *ExtensionLayerTag.ToString()));
		};

		TSharedRef<SOverlay> MessageBox = SNew(SOverlay);

		MessageBox->AddSlot()
			.Padding(5.0f)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Justification(ETextJustify::Center)
				.Text_Lambda(GetExtensionPointText)
			];

		return MessageBox;
	}
	else
	{
		return Super::RebuildWidget();
	}
}

UUserWidget* UExtensionWidget::AddWidget(TSubclassOf<UCommonUserWidget> Widget, APlayerController* OwningPlayer)
{
	if (Widget)
	{
		// Add the widget to the overlay
		UUserWidget* WidgetObject = CreateEntryInternal(Widget);
		WidgetObject->SetOwningPlayer(OwningPlayer);

		return WidgetObject;

	}
	return nullptr;
}

