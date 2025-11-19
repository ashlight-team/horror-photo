// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ContextAction.h"
#include "ItemCustomDataValue.h"
#include "ItemDefinition.h"
#include "ProjectHorrorPhoto/Components/Inventory/Interfaces/ContextActionProvider.h"
#include "UObject/Object.h"
#include "InventoryItemInstance.generated.h"

UCLASS(BlueprintType)
class UInventoryItemInstance : public UObject, public IContextActionProvider
{
	GENERATED_BODY()

public:
	UInventoryItemInstance();
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<class UItemDefinition> ItemDef;
	
	UPROPERTY(BlueprintReadWrite)
	int32 StackCount = 1;

	UPROPERTY(BlueprintReadWrite)
	int32 HotbarIndex = -1;

	UFUNCTION(BlueprintPure)
	float GetUnitWeight() const;
	
	UPROPERTY()
	TArray<FContextAction> RegisteredActions;
	
	UFUNCTION(BlueprintCallable, Category="Item|Actions")
	void RegisterActionBP(FGameplayTag ActionTag, FName ActionId, FText DisplayName, UObject* Target, FName FunctionName, UTexture2D* Icon = nullptr, bool bEnabled = true)
	{
		FContextAction A;
		A.ActionTag = ActionTag;
		A.ActionId = ActionId;
		A.DisplayName = DisplayName;
		A.Icon = Icon;
		A.bEnabled = bEnabled;
		A.Target = Target;
		A.FunctionName = FunctionName;
		RegisteredActions.Add(MoveTemp(A));
	}

	// IContextActionProvider
	virtual TArray<FContextAction> GetContextActions_Implementation() const override
	{
		return RegisteredActions;
	}

	virtual bool ExecuteAction_Implementation(const FGameplayTag& ActionTag, FName ActionId,
	                                          const FContextActionContext& Context) override
	{
		for (const FContextAction& A : RegisteredActions)
		{
			if (A.ActionTag == ActionTag && (ActionId.IsNone() || A.ActionId == ActionId))
				return ExecuteContextAction(A, Context);
		}
		return false;
	}

	bool CanStackWith(const UInventoryItemInstance* Other) const;

	UFUNCTION(BlueprintCallable)
	bool IsIdenticalTo(const UInventoryItemInstance* Other) const;
	
	void InitFromInstance(const UInventoryItemInstance* Other);

public:
	UPROPERTY()
	TMap<FName, FItemCustomDataValue> CustomData;

	UFUNCTION(BlueprintCallable)
	void SetCustomDataValue(FName Key, const FItemCustomDataValue& Value);

	UFUNCTION(BlueprintCallable)
	bool GetCustomDataValue(FName Key, FItemCustomDataValue& OutValue) const;

	UFUNCTION(BlueprintCallable)
	bool RemoveCustomDataValue(FName Key);
};
