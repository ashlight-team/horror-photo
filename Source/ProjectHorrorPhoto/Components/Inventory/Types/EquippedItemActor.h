// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EquippedItemActor.generated.h"

UCLASS()
class PROJECTHORRORPHOTO_API AEquippedItemActor : public AActor
{
	GENERATED_BODY()

public:
	AEquippedItemActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UChildActorComponent* ViewChild;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class UInventoryItemInstance> LinkedInstance;

	void InitFromInstance(class UInventoryItemInstance* Instance);
};
