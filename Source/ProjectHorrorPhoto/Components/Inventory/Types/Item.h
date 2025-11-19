// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectHorrorPhoto/Components/Inventory/Interfaces/ContextActionProvider.h"
#include "Item.generated.h"

UCLASS()
class PROJECTHORRORPHOTO_API AItem : public AActor, public IContextActionProvider
{
	GENERATED_BODY()

public:
	AItem();

	virtual void Destroyed() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class AItemViewActor* ViewActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item")
	class UItemDefinition* DefaultItemDef;
	
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class UItemDefinition> ItemDef;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class UInventoryItemInstance> ItemInstance;
	
	
	UPROPERTY()
	TArray<struct FContextAction> WorldActions;

	UFUNCTION(BlueprintCallable, Category="Item")
	virtual void InitializeFromInstance(UInventoryItemInstance* Instance);

	UFUNCTION(BlueprintCallable, Category="Item")
	virtual void Pickup(AActor* Picker);


	UFUNCTION(BlueprintCallable, Category="Item|Actions")
	void RegisterActionBP(FGameplayTag ActionTag, FName ActionId, FText DisplayName, UObject* Target,
	                      FName FunctionName, UTexture2D* Icon = nullptr, bool bEnabled = true);


	// IContextActionProvider
	virtual TArray<FContextAction> GetContextActions_Implementation() const override;
	virtual bool ExecuteAction_Implementation(const FGameplayTag& ActionTag, FName ActionId,
	                                          const FContextActionContext& Context) override;

	
	UFUNCTION(BlueprintCallable, Category="Item")
	AItemViewActor* GetItemViewActor() const;

public:
	UFUNCTION(BlueprintNativeEvent, Category="Item|Actions")
	void BP_OnPickup(const FContextActionContext& Context);

	virtual void BP_OnPickup_Implementation(const FContextActionContext& Context);

	UFUNCTION(BlueprintNativeEvent, Category="Item|Actions")
	void BP_OnEquip(const FContextActionContext& Context);

	virtual void BP_OnEquip_Implementation(const FContextActionContext& Context);

	UFUNCTION(BlueprintNativeEvent, Category="Item|Actions")
	void BP_OnGrab(const FContextActionContext& Context);

	virtual void BP_OnGrab_Implementation(const FContextActionContext& Context);

	UFUNCTION(BlueprintCallable, Category="Item|Actions")
	void RegisterDefaultActions();
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* Mesh;

private:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	bool bRegisteredDefaultActions = false;
	
	void ApplyVisuals();
private:
	bool bTouchedByCharacter;


};
