// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "PhotoPlayerCharacter.generated.h"

UCLASS()
class PROJECTHORRORPHOTO_API APhotoPlayerCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	APhotoPlayerCharacter();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InputSystem|Abilities")
	class UPhotoAbilitySet* AbilitySet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InputSystem|Input")
	class UPhotoInputConfig* InputConfig;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InputSystem|Input")
	class UInputMappingContext* DefaultMappingContext;


	UFUNCTION(BlueprintCallable, Category="Rag doll")
	bool ToggleRagdoll();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class UInventoryHotbar> InventoryHotbarClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class UCommonActivatableWidget> BasicHUDLayoutWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class UContextMenu> ContextMenuWidgetClass;

private:
	virtual void BeginPlay() override;

	UFUNCTION()
	void Move(const FInputActionValue& Value);

	UFUNCTION()
	void Look(const FInputActionValue& Value);

	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY()
	TWeakObjectPtr<class UPhotoAbilitySystemComponent> AbilitySystemComponent;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void InputAbilityInputTagPressed(FGameplayTag InputTag);
	void InputAbilityInputTagReleased(FGameplayTag InputTag);

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

private:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta= (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComponent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta= (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArmComponent;


	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta= (AllowPrivateAccess = "true"))
	class UInventoryComponent* InventoryComponent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta= (AllowPrivateAccess = "true"))
	class UInventoryManipulatorComponent* InventoryManipulatorComponent;

	UPROPERTY()
	class UInventoryHotbar* InventoryHotbarWidget;

private:
	UFUNCTION()
	void OnHotbar1(const FInputActionValue& Value) { OnHotbarInput(Value, 0); }

	UFUNCTION()
	void OnHotbar2(const FInputActionValue& Value) { OnHotbarInput(Value, 1); }

	UFUNCTION()
	void OnHotbar3(const FInputActionValue& Value) { OnHotbarInput(Value, 2); }

	UFUNCTION()
	void OnHotbar4(const FInputActionValue& Value) { OnHotbarInput(Value, 3); }

	UFUNCTION()
	void OnHotbar5(const FInputActionValue& Value) { OnHotbarInput(Value, 4); }

	UFUNCTION()
	void OnHotbar6(const FInputActionValue& Value) { OnHotbarInput(Value, 5); }

	UFUNCTION()
	void OnHotbar7(const FInputActionValue& Value) { OnHotbarInput(Value, 6); }

	UFUNCTION()
	void OnHotbar8(const FInputActionValue& Value) { OnHotbarInput(Value, 7); }

	UFUNCTION()
	void OnHotbar9(const FInputActionValue& Value) { OnHotbarInput(Value, 8); }

	void OnHotbarInput(const FInputActionValue& Value, int32 SlotIndex);

	void OnRotateModePressed(const FInputActionValue& Value);

	void OnRotateObject(const FInputActionValue& Value);

	void OnTryToOpenContextMenuForLookTarget(const FInputActionValue& Value);

	void OnCloseContextMenu(const FInputActionValue& Value);

private:
	UPROPERTY(meta=(AllowPrivateAccess = true))
	float LookForwardTraceDistance = 500.0f;

	UPROPERTY()
	TObjectPtr<class UContextMenu> CachedContextMenu = nullptr;
};
