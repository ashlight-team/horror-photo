// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "Components/DynamicEntryBoxBase.h"
#include "ExtensionWidget.generated.h"


/**
 *
 */
UCLASS()
class PROJECTHORRORPHOTO_API UExtensionWidget : public UDynamicEntryBoxBase
{
	GENERATED_BODY()

public:
	UExtensionWidget(const FObjectInitializer& ObjectInitializer);

	virtual TSharedRef<SWidget> RebuildWidget() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (Categories = "UI.Widget"))
	FGameplayTag ExtensionLayerTag;

	virtual UUserWidget* AddWidget(TSubclassOf<UCommonUserWidget> Widget, APlayerController* OwningPlayer);

};
