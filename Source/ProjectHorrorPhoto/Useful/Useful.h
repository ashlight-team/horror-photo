// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ProjectHorrorPhoto/AbilitySystem/Component/PhotoAbilitySystemComponent.h"
#include "Useful.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTHORRORPHOTO_API UUseful : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Useful")
	static USceneComponent* FindChildComponentByTag(AActor* Actor, FName Tag);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Useful")
	static UPhotoAbilitySystemComponent* GetPhotoAbilitySystemComponent(AActor* Actor);
};
