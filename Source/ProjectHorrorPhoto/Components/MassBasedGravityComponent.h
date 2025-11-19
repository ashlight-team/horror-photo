#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MassBasedGravityComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTHORRORPHOTO_API UMassBasedGravityComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMassBasedGravityComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gravity")
	float GravityStrength = 980.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gravity")
	bool bOverrideDefaultGravity = true;
};
