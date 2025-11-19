// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerController.h"
#include "PhotoPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTHORRORPHOTO_API APhotoPlayerController : public APlayerController, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

private:
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	
};
