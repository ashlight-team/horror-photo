// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "PhotoAttributeSet.h"
#include "MovementAttributeSet.generated.h"


UCLASS()
class PROJECTHORRORPHOTO_API UMovementAttributeSet : public UPhotoAttributeSet
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category="Movement", ReplicatedUsing=OnRep_WalkSpeed)
	FGameplayAttributeData WalkSpeed;
	ATTRIBUTE_ACCESSORS(UMovementAttributeSet, WalkSpeed)

	UPROPERTY(BlueprintReadOnly, Category="Movement", ReplicatedUsing=OnRep_RunSpeedAdditive)
	FGameplayAttributeData RunSpeedAdditive;
	ATTRIBUTE_ACCESSORS(UMovementAttributeSet, RunSpeedAdditive)

	UPROPERTY(BlueprintReadOnly, Category="Movement", ReplicatedUsing=OnRep_JumpHeight)
	FGameplayAttributeData JumpHeight;
	ATTRIBUTE_ACCESSORS(UMovementAttributeSet, JumpHeight)

	UPROPERTY(BlueprintReadOnly, Category="Movement", ReplicatedUsing=OnRep_ItemDragCoefficient)
	FGameplayAttributeData ItemDragCoefficient;
	ATTRIBUTE_ACCESSORS(UMovementAttributeSet, ItemDragCoefficient)

private:
	UFUNCTION()
	void OnRep_WalkSpeed(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(UMovementAttributeSet, WalkSpeed, OldValue); }

	UFUNCTION()
	void OnRep_JumpHeight(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(UMovementAttributeSet, JumpHeight, OldValue); }

	UFUNCTION()
	void OnRep_ItemDragCoefficient(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(UMovementAttributeSet, ItemDragCoefficient, OldValue); }

	UFUNCTION()
	void OnRep_RunSpeedAdditive(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(UMovementAttributeSet, RunSpeedAdditive, OldValue); }

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
