// Fill out your copyright notice in the Description page of Project Settings.


#include "Useful.h"

#include "AbilitySystemInterface.h"

USceneComponent* UUseful::FindChildComponentByTag(AActor* Actor, FName Tag)
{
	if (!Actor) return nullptr;

	TArray<UActorComponent*> Components = Actor->GetComponentsByTag(USceneComponent::StaticClass(), Tag);
	if (Components.Num() > 0)
	{
		return Cast<USceneComponent>(Components[0]);
	}

	return nullptr;
}

UPhotoAbilitySystemComponent* UUseful::GetPhotoAbilitySystemComponent(AActor* Actor)
{
	if (!Actor) return nullptr;
	if (Actor->GetClass()->ImplementsInterface(UAbilitySystemInterface::StaticClass()))
	{
		IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(Actor);
		if (AbilitySystemInterface)
		{
			if (UAbilitySystemComponent* ASC = AbilitySystemInterface->GetAbilitySystemComponent())
			{
				return Cast<UPhotoAbilitySystemComponent>(ASC);
			}
		}
	}
	return nullptr;
}
