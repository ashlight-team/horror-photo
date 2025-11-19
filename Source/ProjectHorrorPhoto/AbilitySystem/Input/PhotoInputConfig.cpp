// Fill out your copyright notice in the Description page of Project Settings.


#include "PhotoInputConfig.h"


const UInputAction* UPhotoInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag) const
{
	for (const FPhotoInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}
	UE_LOG(LogTemp, Error, TEXT("Can't find NativeInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	
	return nullptr;
}

const UInputAction* UPhotoInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag) const
{
	for (const FPhotoInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}
	UE_LOG(LogTemp, Error, TEXT("Can't find NativeInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	
	return nullptr;
}