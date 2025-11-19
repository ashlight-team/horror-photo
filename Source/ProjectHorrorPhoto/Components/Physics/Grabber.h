// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Grabber.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGrabbedItemChanged, AActor*, NewGrabbedItem);
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTHORRORPHOTO_API UGrabber : public UActorComponent
{
	GENERATED_BODY()

public:
	UGrabber();

	UFUNCTION(BlueprintCallable, Category="Grab")
	bool Grab(AActor* WorldActor, const FHitResult& HitResult);

	UFUNCTION(BlueprintCallable, Category="Grab")
	void Rotate(FVector2D Delta);

	UFUNCTION(BlueprintCallable, Category="Grab")
	void DropGrabbed();

	UFUNCTION(BlueprintCallable, Category="Grab")
	void ThrowGrabbed(float ThrowForce = 1000.f);

	UFUNCTION(BlueprintPure, Category="Grab")
	bool HasGrabbed() const { return GrabbedActor != nullptr; }

public:
	UPROPERTY(BlueprintAssignable, Category="Grab")
	FOnGrabbedItemChanged OnGrabbedItemChanged;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grab")
	float HoldDistance = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grab")
	float MaxHoldDistance = 400.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grab")
	float MaxGrabMass = 80.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grab")
	float BaseInterpSpeed = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grab")
	float MassFactor = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grab")
	float BaseThrowForce = 1500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grab")
	float RotationSpeed = 2.0f;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Grab", meta=(AllowPrivateAccess="true"))
	class UPhysicsHandleComponent* PhysicsHandle;

	UPROPERTY()
	AActor* GrabbedActor;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void BeginPlay() override;
};
