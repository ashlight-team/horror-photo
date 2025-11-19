// Fill out your copyright notice in the Description page of Project Settings.


#include "PhotoPlayerController.h"

#include "PhotoPlayerState.h"
#include "ProjectHorrorPhoto/AbilitySystem/Component/PhotoAbilitySystemComponent.h"

UAbilitySystemComponent* APhotoPlayerController::GetAbilitySystemComponent() const
{
	if (APhotoPlayerState* PhotoPlayerState = GetPlayerState<APhotoPlayerState>())
	{
		return PhotoPlayerState->GetAbilitySystemComponent();
	}
	return nullptr;
}

void APhotoPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if(UPhotoAbilitySystemComponent* ASC = Cast<UPhotoAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		ASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}
	
	Super::PostProcessInput(DeltaTime, bGamePaused);
	
}
