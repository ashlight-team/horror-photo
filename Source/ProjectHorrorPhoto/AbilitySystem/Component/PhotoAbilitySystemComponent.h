#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"

#include "PhotoAbilitySystemComponent.generated.h"

UCLASS()
class PROJECTHORRORPHOTO_API UPhotoAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UPhotoAbilitySystemComponent();

	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
	void ClearAbilityInput();

protected:
	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

protected:
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;

	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;
};
