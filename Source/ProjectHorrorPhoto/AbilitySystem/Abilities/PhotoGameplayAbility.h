#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "PhotoGameplayAbility.generated.h"

/**
 * Defines how ability is meant to activate
 */
UENUM(BlueprintType)
enum class EISAbilityActivationPolicy : uint8
{
	OnInputTriggered,
	WhileInputActive,
};


UCLASS()
class PROJECTHORRORPHOTO_API UPhotoGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	EISAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }

	UFUNCTION(BlueprintImplementableEvent, Category = "SN|Ability Activation")
	void OnInputReleased();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SN|Ability Activation")
	EISAbilityActivationPolicy ActivationPolicy;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
};
