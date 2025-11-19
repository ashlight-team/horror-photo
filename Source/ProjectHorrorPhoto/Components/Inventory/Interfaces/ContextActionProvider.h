// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ProjectHorrorPhoto/Components/Inventory/Types/ContextAction.h"
#include "UObject/Interface.h"
#include "ContextActionProvider.generated.h"

UINTERFACE(Blueprintable)
class PROJECTHORRORPHOTO_API UContextActionProvider : public UInterface
{
	GENERATED_BODY()
};

class IContextActionProvider
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TArray<FContextAction> GetContextActions() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool ExecuteAction(const FGameplayTag& ActionTag, FName ActionId, const FContextActionContext& Context);
};