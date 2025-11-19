// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "ContextAction.generated.h"

USTRUCT(BlueprintType)
struct FContextAction
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag ActionTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ActionId;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Icon = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnabled = true;

	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<UObject> Target;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName FunctionName = NAME_None;
};

USTRUCT(BlueprintType)
struct FContextActionContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	AActor* Caller = nullptr;
	
	UPROPERTY(BlueprintReadOnly)
	AActor* SourceActor = nullptr;
	
	UPROPERTY(BlueprintReadOnly)
	class UInventoryItemInstance* SourceItem = nullptr; 
	
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag ActionTag;
	
	UPROPERTY(BlueprintReadOnly)
	FName ActionId;
};

inline bool ExecuteContextAction(const FContextAction& Action, const FContextActionContext& Ctx)
{
	if (!Action.bEnabled || !Action.Target.IsValid() || Action.FunctionName.IsNone())
		return false;

	UObject* TargetObj = Action.Target.Get();
	if (UFunction* Func = TargetObj->FindFunction(Action.FunctionName))
	{
		struct
		{
			FContextActionContext Context;
		} Params{Ctx};
		TargetObj->ProcessEvent(Func, &Params);
		return true;
	}
	return false;
}
