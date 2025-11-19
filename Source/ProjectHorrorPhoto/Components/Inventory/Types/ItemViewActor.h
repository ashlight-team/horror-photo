// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemViewActor.generated.h"

UCLASS(Blueprintable)
class PROJECTHORRORPHOTO_API AItemViewActor : public AActor
{
	GENERATED_BODY()

public:
	AItemViewActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Mesh;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class UInventoryItemInstance> LinkedInstance;

	UFUNCTION(BlueprintCallable)
	virtual void InitFromInstance(class UInventoryItemInstance* Instance);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ApplyVisuals();
	

};
