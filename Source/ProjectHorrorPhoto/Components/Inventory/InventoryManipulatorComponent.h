#pragma once

#include "CoreMinimal.h"
#include "FCTweenInstance.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "ProjectHorrorPhoto/AbilitySystem/Input/PhotoAbilitySet.h"
#include "Types/EquippedItemActor.h"

#include "InventoryManipulatorComponent.generated.h"

UENUM()
enum EGrabMode
{
	Grab,
	Drag,
};

UENUM()
enum class EItemSource : uint8
{
	None,
	Inventory,
	Hotbar
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHotbarUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHotbarSlotUpdated, int32, SlotIndex, UInventoryItemInstance*, NewItem);

class UInventoryItemInstance;
class UInventoryComponent;
class UGrabber;
class UDragger;
class UPhysicsHandleComponent;
class AItem;
class AEquippedItemActor;

UCLASS(Blueprintable, ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent))
class PROJECTHORRORPHOTO_API UInventoryManipulatorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryManipulatorComponent();

	// =========================
	// config / tuning (edit in BP)
	// =========================

	// inventory stuff
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	int32 MaxHotbarSlots = 9;

	// ray / spawn distances
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory")
	float HitDistance = 300.f;

	// grab/throw tuning
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grab")
	float MaxGrabMass = 80.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grab")
	float MassFactor = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grab")
	float BaseThrowForce = 1500.f;

	// drag tuning
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drag")
	float MaxDragMass = 250.f;

	// weight limits
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weight")
	float MaxHotbarWeight = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weight")
	float MaxHandItemWeight = 10.f;

	// =========================
	// runtime state (what we have now)
	// =========================

	// hotbar slots (array of stacks)
	UPROPERTY(BlueprintReadOnly, Category="Hotbar")
	TArray<TObjectPtr<UInventoryItemInstance>> HotbarItems;

	// what we hold now (separate instance)
	UPROPERTY(BlueprintReadOnly, Category="Equip")
	UInventoryItemInstance* EquippedItem = nullptr;

	// legacy? keep if used somewhere
	UPROPERTY(BlueprintReadOnly, Category="Equip")
	TObjectPtr<UInventoryItemInstance> HeldItem;

	// where item came from (for auto-return)
	UPROPERTY()
	int32 EquippedFromSlot = INDEX_NONE;

	// last used hotbar slot (if you still need it)
	UPROPERTY()
	int32 LastEquippedHotbarIndex = -1;

	// visual actor in hands
	UPROPERTY()
	AEquippedItemActor* EquippedVisual = nullptr;

	FPhotoAbilitySet_GrantedHandles* EquippedGrantedHandles;

	// grab/drag state
	UPROPERTY(BlueprintReadOnly, Category="Grab")
	AItem* GrabbedItem = nullptr;

private:
	// simple mode flag
	EGrabMode GrabMode;

public:
	// source info if needed
	UPROPERTY(BlueprintReadOnly, Category="Equip")
	EItemSource EquippedSource;

	// helper components
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Grab")
	UGrabber* Grabber;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Drag")
	UDragger* Dragger;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Grab")
	UPhysicsHandleComponent* PhysicsHandle;

	// =========================
	// events (bind UI)
	// =========================
	UPROPERTY(BlueprintAssignable, Category="Inventory")
	FOnHotbarUpdated OnHotbarUpdated;

	UPROPERTY(BlueprintAssignable, Category="Inventory")
	FOnHotbarSlotUpdated OnHotbarSlotUpdated;

	// =========================
	// public API — hands / equip
	// =========================

	// take one (consume from inv stack)
	UFUNCTION(BlueprintCallable, Category="Equip")
	bool TakeToHands(UInventoryItemInstance* Item);

	// hold exact instance (already split/made)
	UFUNCTION(BlueprintCallable, Category="Equip")
	bool HoldInHands(UInventoryItemInstance* Item);

	// move from hands back to inventory
	UFUNCTION(BlueprintCallable, Category="Equip")
	bool PutFromHandsToInventory();

	// safe put to inventory (no duplicates)
	void PutToInventory(UInventoryItemInstance* Item);

	// drop/throw/equip from grabbed
	UFUNCTION(BlueprintCallable, Category="Equip")
	bool DropEquippedItem(const FTransform& SpawnXform);

	UFUNCTION(BlueprintCallable, Category="Equip")
	bool DropEquippedItemInFrontOfPlayer();
	
	bool DropInstanceInFrontOfPlayer(UInventoryItemInstance* Instance, float ForwardDist = 100.f);

	UFUNCTION(BlueprintCallable, Category="Equip")
	bool ThrowEquippedItem(float ThrowForce = 1000.f);

	UFUNCTION(BlueprintCallable, Category="Equip")
	bool EquipGrabbed();

	// =========================
	// public API — hotbar
	// =========================

	// activate slot (take/return/swap rules)
	UFUNCTION(BlueprintCallable, Category="Hotbar")
	bool ActivateHotbarSlot(int32 SlotIndex);

	// assign exact item to slot (merge if identical)
	UFUNCTION(BlueprintCallable, Category="Hotbar")
	bool AssignHotbarSlot(int32 SlotIndex, UInventoryItemInstance* Item);

	// get slot item ptr
	UFUNCTION(BlueprintCallable, Category="Hotbar")
	UInventoryItemInstance* GetHotbarItem(int32 SlotIndex);

	// second tap on same slot: return/merge/inv
	UFUNCTION(BlueprintCallable, Category="Hotbar")
	bool ReturnHeldToSlotPolicy(int32 SlotIndex);

	// split from stack (Qty) or move whole if equal
	UFUNCTION(BlueprintCallable, Category="Hotbar")
	UInventoryItemInstance* SplitQtyFromStack(UInventoryItemInstance* Source, int32 Qty);

	/// ========================
	/// public API - weight
	/// ========================
	UFUNCTION(BlueprintPure, Category="Weight")
	float GetHotbarWeight() const;

	UFUNCTION(BlueprintPure, Category="Weight")
	float GetEquippedWeight() const;

	UFUNCTION(BlueprintPure, Category="Weight")
	float GetCarriedWeight() const;    
	
	UFUNCTION(BlueprintPure, Category="Weight")
	bool CanCarryDeltaInHands(float Delta);

	UFUNCTION(BlueprintPure, Category="Weight")
	bool CanCarryDeltaInHotbar(float Delta);

	UFUNCTION(BlueprintPure, Category="Weight")
	bool CanCarryItemInHotbar(const UInventoryItemInstance* Item) const;

	UFUNCTION(BlueprintPure, Category="Weight")
	bool CanHoldInHands(const UInventoryItemInstance* Item) const;

	// =========================
	// public API — grab/drag
	// =========================

	// try grab or drag based on mass
	UFUNCTION(BlueprintCallable, Category="Grab")
	bool DragOrGrabItem(AItem* WorldItem);

	// release grabbed/dragged
	UFUNCTION(BlueprintCallable, Category="Grab")
	void DropGrabbedItem();

	// throw what you grabbed
	UFUNCTION(BlueprintCallable, Category="Grab")
	void ThrowGrabbedItem(float ThrowForce = 1000.f);

	// quick check
	UFUNCTION(BlueprintPure, Category="Grab")
	bool HasGrabbedItem() const { return GrabbedItem != nullptr; }

	// make item from hands become grabbed (spawn to world then grab)
	UFUNCTION(BlueprintCallable, Category="Grab")
	bool GrabEquipped();

	// =========================
	// actions (context system)
	// =========================

	UFUNCTION(BlueprintCallable, Category="Actions")
	bool ExecuteHeldItemAction(const FGameplayTag& ActionTag, FName ActionId);

	UFUNCTION(BlueprintCallable, Category="Actions")
	bool ExecuteWorldTargetAction(AActor* TargetActor, const FGameplayTag& ActionTag, FName ActionId);

	// =========================
	// visuals (spawn/destroy fp model)
	// =========================

	UFUNCTION(BlueprintCallable, Category="Visual")
	void MoveItemInFrontOfPlayer(bool bInFront = true);

	void UpdateEquippedVisual();
	void DestroyEquippedVisual();

private:
	// internal callback from grab/drag comps
	UFUNCTION()
	void OnGrabbedOrDraggedActorChanged(AActor* NewGrabbedItem);

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
