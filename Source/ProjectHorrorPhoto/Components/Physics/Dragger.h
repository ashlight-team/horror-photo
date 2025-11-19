// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Dragger.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDraggedItemChanged, AActor*, NewDraggedItem);
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTHORRORPHOTO_API UDragger : public UActorComponent
{
	GENERATED_BODY()

public:
	UDragger();

	UFUNCTION(BlueprintCallable, Category="Grab")
	bool DragItem(AActor* WorldActor, const FHitResult& HitResult);

	UFUNCTION(BlueprintCallable, Category="Grab")
	void DropDragged();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drag")
	float MaxDragMass = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drag")
	float DragDistance = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drag")
	float MaxDragDistance = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drag")
	float Slack = 5.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drag")
	float SpringK = 40000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drag")
	float DampingC = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drag")
	float MaxHandForceN = 1500.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drag")
	float GravityScaleWhileGrab = 2.f;

public:
 	UPROPERTY(BlueprintAssignable, Category="Drag")
	FOnDraggedItemChanged OnDraggedItemChanged;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Grab", meta=(AllowPrivateAccess="true"))
	class UPrimitiveComponent* HandAnchor = nullptr;

	UPROPERTY()
	FVector DragOffset;

	UPROPERTY()
	class AActor* DraggedItem = nullptr;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void BeginPlay() override;

};
