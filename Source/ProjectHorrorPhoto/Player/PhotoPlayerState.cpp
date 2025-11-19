// Fill out your copyright notice in the Description page of Project Settings.


#include "PhotoPlayerState.h"

#include "ProjectHorrorPhoto/AbilitySystem/Component/PhotoAbilitySystemComponent.h"

APhotoPlayerState::APhotoPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UPhotoAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Full);
}

UAbilitySystemComponent* APhotoPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
