// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"

#include "InventoryComponent.generated.h"


class UInventoryItemInstance;

UCLASS(Blueprintable, ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent))
class PROJECTHORRORPHOTO_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weight")
	float MaxInventoryWeight = 90.f;
	
	UFUNCTION(BlueprintPure, Category="Weight")
	float GetCurrentWeight() const;
	
	UFUNCTION(BlueprintPure, Category="Weight")
	bool CanAcceptInstance(const UInventoryItemInstance* Instance) const;
	
	UFUNCTION(BlueprintPure, Category="Weight")
	bool CanAccept(UItemDefinition* Def, int32 Amount) const;

	
	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<class UInventoryItemInstance>> Items;

	UFUNCTION(BlueprintCallable) bool AddItem(class UItemDefinition* Def, int32 Amount = 1);
	UFUNCTION(BlueprintCallable) UInventoryItemInstance* AddOrGetItem(class UItemDefinition* Def, int32 Amount = 1);
	UFUNCTION(BlueprintCallable) bool ConsumeOne(class UInventoryItemInstance* Instance);
	UFUNCTION(BlueprintCallable) bool AddItemInstance(UInventoryItemInstance* Instance);
	UFUNCTION(BlueprintCallable) bool RemoveItemInstance(UInventoryItemInstance* Instance, int32 Amount = 1);
	UFUNCTION(BlueprintCallable) bool DropItemInstance(UInventoryItemInstance* Instance, const FTransform& SpawnTransform, bool bDropAll);


};
