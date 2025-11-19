// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "PhotoPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTHORRORPHOTO_API APhotoPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	APhotoPlayerState();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

private:
	UPROPERTY()
	TObjectPtr<class UPhotoAbilitySystemComponent> AbilitySystemComponent;
};
