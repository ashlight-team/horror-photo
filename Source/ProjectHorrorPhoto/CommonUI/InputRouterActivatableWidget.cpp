// Fill out your copyright notice in the Description page of Project Settings.


#include "InputRouterActivatableWidget.h"

TOptional<FUIInputConfig> UInputRouterActivatableWidget::GetDesiredInputConfig() const
{
	switch (InputConfig)
	{
	case EInputRouterInputMode::GameAndMenu:
		return FUIInputConfig(ECommonInputMode::All, GameMouseCaptureMode);
	case EInputRouterInputMode::Game:
		return FUIInputConfig(ECommonInputMode::Game, GameMouseCaptureMode);
	case EInputRouterInputMode::Menu:
		return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture);
	case EInputRouterInputMode::Default:
	default:
		return TOptional<FUIInputConfig>();
	}
}
