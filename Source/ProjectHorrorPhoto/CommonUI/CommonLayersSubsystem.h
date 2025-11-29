// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "ExtensionWidget.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "CommonLayersSubsystem.generated.h"

/**
 *
 */
UCLASS()
class PROJECTHORRORPHOTO_API UCommonLayersSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	virtual void RegisterLayer(UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag Tag, UCommonActivatableWidgetStack* Layer, ESlateVisibility Visibility = ESlateVisibility::Visible);

	UFUNCTION(BlueprintCallable, Category = "UI")
	virtual void UnregisterLayer(UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag Tag);

	UFUNCTION(BlueprintCallable, Category = "UI", meta = (WidgetClass = "/Script/CommonUI.CommonActivatableWidget", DeterminesOutputType = "WidgetClass"))
	virtual UCommonActivatableWidget* PushWidgetToLayer(UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag Tag, TSubclassOf<UCommonActivatableWidget> WidgetClass);

	UFUNCTION(BlueprintCallable, Category = "UI", meta = (WidgetClass = "/Script/CommonUI.CommonActivatableWidget", DeterminesOutputType = "WidgetClass"))
	virtual UCommonActivatableWidget* PushWidgetToLayer_ForPlayer(APlayerController* OwningPlayer, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag Tag, TSubclassOf<UCommonActivatableWidget> WidgetClass);
	

	UFUNCTION(BlueprintCallable, Category = "UI")
	virtual void RegisterExtensionLayer(UPARAM(meta = (Categories = "UI.Widget")) FGameplayTag Tag, UExtensionWidget* Layer);

	UFUNCTION(BlueprintCallable, Category = "UI")
	virtual void UnregisterExtensionLayer(UPARAM(meta = (Categories = "UI.Widget")) FGameplayTag Tag);

	UFUNCTION(BlueprintCallable, Category = "UI", meta = (WidgetClass = "/Script/UMG.UserWidget", DeterminesOutputType = "WidgetClass"))
	virtual UUserWidget* PushWidgetToExtensionLayer(APlayerController* OwningPlayer, UPARAM(meta = (Categories = "UI.Widget")) FGameplayTag Tag, TSubclassOf<UCommonUserWidget> WidgetClass);

protected:
	UPROPERTY()
	TMap<FGameplayTag, UCommonActivatableWidgetStack*> Layers;

	UPROPERTY()
	TMap<FGameplayTag, ESlateVisibility> LayerVisibility;

	UPROPERTY()
	TMap<FGameplayTag, UExtensionWidget*> ExtensionLayers;
};
