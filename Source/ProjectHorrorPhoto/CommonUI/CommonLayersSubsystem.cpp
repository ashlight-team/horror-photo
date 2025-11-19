// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonLayersSubsystem.h"

#include "ExtensionWidget.h"

void UCommonLayersSubsystem::RegisterLayer(FGameplayTag Tag, UCommonActivatableWidgetStack* Layer, ESlateVisibility Visibility)
{
	Layers.Add(Tag, Layer);
	LayerVisibility.Add(Tag, Visibility);
}

void UCommonLayersSubsystem::UnregisterLayer(FGameplayTag Tag)
{
	Layers.Remove(Tag);
}

UCommonActivatableWidget* UCommonLayersSubsystem::PushWidgetToLayer(FGameplayTag Tag,
	TSubclassOf<UCommonActivatableWidget> WidgetClass)
{
	auto Layer = Layers.Find(Tag);
	if (Layer)
	{
		auto Widget = Layers[Tag]->AddWidget(WidgetClass);
		ESlateVisibility Visibility = LayerVisibility[Tag];
		Layers[Tag]->SetVisibility(Visibility);
		return Widget;
	}
	return nullptr;
}

UCommonActivatableWidget* UCommonLayersSubsystem::PushWidgetToLayer_ForPlayer(APlayerController* OwningPlayer,
	FGameplayTag Tag, TSubclassOf<UCommonActivatableWidget> WidgetClass)
{
	if (!ensure(OwningPlayer) || !ensure(WidgetClass))
	{
		return nullptr;
	}
	UCommonActivatableWidget* Widget = PushWidgetToLayer(Tag, WidgetClass);
	Widget->SetOwningPlayer(OwningPlayer);
	return Widget;
}

void UCommonLayersSubsystem::RegisterExtensionLayer(FGameplayTag Tag, UExtensionWidget* Layer)
{
	ExtensionLayers.Add(Tag, Layer);
}

void UCommonLayersSubsystem::UnregisterExtensionLayer(FGameplayTag Tag)
{
	ExtensionLayers.Remove(Tag);
}

UUserWidget* UCommonLayersSubsystem::PushWidgetToExtensionLayer(APlayerController* OwningPlayer, FGameplayTag Tag, TSubclassOf<UCommonUserWidget> WidgetClass)
{
	auto Layer = ExtensionLayers.Find(Tag);
	if (Layer)
	{
		TObjectPtr<UUserWidget> NewWidget = ExtensionLayers[Tag]->AddWidget(WidgetClass, OwningPlayer);
		// GEngine debug log widget name
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Added new widget to extension layer: %s"), *WidgetClass->GetName()));

		return NewWidget;

	}
	return nullptr;
}
