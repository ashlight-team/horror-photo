// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "InputRouterActivatableWidget.generated.h"

UENUM(BlueprintType)
enum class EInputRouterInputMode : uint8
{
	Default,
	GameAndMenu,
	Game,
	Menu
};


/**
 * 
 */
UCLASS()
class PROJECTHORRORPHOTO_API UInputRouterActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

protected:
	/** The desired input mode to use while this UI is activated, for example do you want key presses to still reach the game/player controller? */
	UPROPERTY(EditDefaultsOnly, Category = Input)
	EInputRouterInputMode InputConfig = EInputRouterInputMode::Default;

	/** The desired mouse behavior when the game gets input. */
	UPROPERTY(EditDefaultsOnly, Category = Input)
	EMouseCaptureMode GameMouseCaptureMode = EMouseCaptureMode::CapturePermanently;
};
